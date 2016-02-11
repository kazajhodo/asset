<?php

/**
 * Implements hook_path_alias_types().
 */
function asset_path_alias_types() {
  $objects['asset/'] = t('Asset');
  return $objects;
}

/**
 * Implements hook_pathauto().
 */
function asset_pathauto($op) {
  switch ($op) {
    case 'settings':
      $entity_type = 'asset';
      $entity_info = entity_get_info($entity_type);

      $settings[$entity_type] = (object) array(
        'module' => $entity_type,
        'token_type' => $entity_info['token type'],
        'groupheader' => t('!entity_type paths', array(
          '!entity_type' => $entity_info['label'],
        )),
        'patterndescr' => t('Default path pattern (applies to all !entity_type entity types with blank patterns below)', array(
          '!entity_type' => $entity_info['label'],
        )),
        'patterndefault' => '',
        'batch_update_callback' => 'asset_bulk_update_batch_callback',
        'batch_file' => drupal_get_path('module', 'asset') . '/includes/asset.pathauto.inc',
      );

      // Sort through each Entity Type to add support for bundles
      foreach (asset_get_types() as $type) {
        if($type->data['viewable'] && $type->data['pathauto']){
          $settings[$entity_type]->patternitems[$type->type] = t('Pattern for all @bundle paths', array('@bundle' => $type->title));
        }
      }

      return $settings;
    default:
      break;
  }
}

/**
 * Creates or updates entity alias.
 *
 * @param string $entity_type
 *   Type of the entity the alias is being created/updated for.
 * @param object $entity
 *   An entity object the alias is being created/updated for.
 * @param string $op
 *   Operation being performed on the alias ('insert', 'update' or 'bulkupdate').
 * @param string|null $language
 *   Additional parameter to set the language of the path.
 *
 * @return
 *   The alias that was created.
 */
function asset_update_alias($entity_type, $entity, $op, $language = NULL) {

  // Skip processing if the entity has no pattern.
  $entity_info = entity_get_info($entity_type);
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
  if (!pathauto_pattern_load_by_entity($entity_type, $bundle)) {
    return;
  }

  if ($op === 'insert' && $entity_type != 'file') {
    // @todo Remove the next line when http://drupal.org/node/1025870 is fixed.
    unset($entity->uri);
  }

  // Make sure the language is set.
  if (!isset($language)) {
    $language = pathauto_entity_language($entity_type, $entity);
  }

  module_load_include('inc', 'pathauto');
  $uri = entity_uri($entity_type, $entity);

  // Clean
  pathauto_path_delete_all($uri['path']);

  return pathauto_create_alias($entity_type, $op, $uri['path'], array($entity_info['token type'] => $entity), $bundle, $language);
}

/**
 * @file
 * Pathauto Entity batch callback implementations.
 */

/**
 * Batch callback; count the current number of URL aliases for comparison later.
 *
 * Added here only because it needs to be in the same file as main
 * asset_bulk_update_batch_process().
 *
 * @see asset_bulk_update_form_submit()
 */
function asset_bulk_update_batch_start(&$context) {
  module_load_include('inc', 'pathauto', 'pathauto.admin');
  pathauto_bulk_update_batch_start($context);
}

/**
 * Common batch processing callback for entity-based operations.
 *
 * Required to load the proper batch file.
 *
 * Overrides pathauto_bulk_update_batch_process().
 *
 * @see asset_bulk_update_form_submit()
 * @see pathauto_bulk_update_batch_process()
 */
function asset_bulk_update_batch_process($callback, $settings, &$context) {
  if (!empty($settings->batch_file)) {
    require_once DRUPAL_ROOT . '/' . $settings->batch_file;
  }
  // When dealing with entity aliases we also need to pass $settings array to
  // callback function, so it knows which entity types to generate aliases for.
  if ($callback == 'asset_bulk_update_batch_callback') {
    return $callback($settings, $context);
  }
  else {
    return $callback($context);
  }
}

/**
 * Batch processing callback; Generate aliases for entities.
 *
 * Differs from standard bulk update callback functions by having $settings
 * as parameter, which we need to know which entity type should URL aliases
 * be generated for.
 *
 * @see asset_pathauto()
 * @see asset_bulk_update_batch_process()
 */
function asset_bulk_update_batch_callback($settings, &$context) {
  if (!isset($context['sandbox']['count'])) {
    // First pass. Build an array with all the pairs of id and entity type to process.
    $context['sandbox']['count'] = 0;
    $context['sandbox']['items'] = array();

    $entity_type = 'asset';
    $asset_types = array();
    // Sort through each Entity Type to add support for bundles
    foreach (asset_get_types() as $type) {
      if($type->data['viewable'] && $type->data['pathauto']){
        $asset_types[] = $type->type;
      }
    }

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type)->propertyCondition('type', $asset_types);
    $result = $query->execute();
    $ids = isset($result[$entity_type]) ? array_keys($result[$entity_type]) : array();

    foreach ($ids as $id) {
      $context['sandbox']['items'][] = array(
        'id' => $id,
        'type' => $entity_type
      );
    }
    $context['sandbox']['total'] = count($context['sandbox']['items']);

    // If there are no items to update, stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  // Extract 25 items to be processed in this pass.
  $items_to_process = array_slice($context['sandbox']['items'], $context['sandbox']['count'], 25);
  module_load_include('inc', 'pathauto');

  foreach ($items_to_process as $item) {
    $entity = entity_load($item['type'], array($item['id']));
    $entity = reset($entity);
    asset_update_alias($item['type'], $entity, 'bulkupdate');
  }

  // Update progress stats.
  $context['sandbox']['count'] += count($items_to_process);
  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}

/**
 * Batch finished callback.
 *
 * Added here only because it needs to be in the same file as main
 * asset_bulk_update_batch_process().
 *
 * @see asset_bulk_update_form_submit()
 */
function asset_bulk_update_batch_finished($success, $results, $operations) {
  module_load_include('inc', 'pathauto', 'pathauto.admin');
  pathauto_bulk_update_batch_finished($success, $results, $operations);
}
