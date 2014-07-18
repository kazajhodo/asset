<?php

function asset_view_modes(){
  return array('asset' => t('Asset'),'asset_preview' => t('Asset Preview'),'asset_full' => t('Asset Full'));
}

/**
 * Implements hook_field_formatter_info().
 */
function asset_field_formatter_info() {
  return array(
    'asset_entity_view' => array(
      'label' => t('Rendered asset'),
      'description' => t('Display the referenced assets rendered by entity_view().'),
      'field types' => array('entityreference'),
      'settings' => array(
        'view_mode' => 'asset',
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function asset_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  if ($display['type'] == 'asset_entity_view') {
    $entity_info = entity_get_info($field['settings']['target_type']);
    $options = asset_view_modes();
    if (!empty($entity_info['view modes'])) {
      foreach ($entity_info['view modes'] as $view_mode => $view_mode_settings) {
        $options[$view_mode] = $view_mode_settings['label'];
      }
    }

    $element['view_mode'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('View mode'),
      '#default_value' => $settings['view_mode'],
      '#access' => count($options) > 1,
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function asset_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = array();

  if ($display['type'] == 'asset_entity_view') {
    $entity_info = entity_get_info($field['settings']['target_type']);
    $asset_view_modes = asset_view_modes();
    $view_mode_label = isset($asset_view_modes[$settings['view_mode']]) ? $asset_view_modes[$settings['view_mode']] : $settings['view_mode'];
    if (isset($entity_info['view modes'][$settings['view_mode']]['label'])) {
      $view_mode_label = $entity_info['view modes'][$settings['view_mode']]['label'];
    }
    $summary[] = t('Rendered as @mode', array('@mode' => $view_mode_label));
  }

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_prepare_view().
 */
function asset_field_formatter_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items, $displays) {
  $target_ids = array();

  // Collect every possible entity attached to any of the entities.
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      if (is_array($item) && isset($item['target_id'])) {
        $target_ids[] = $item['target_id'];
      }
    }
  }

  if ($target_ids) {
    $target_entities = entity_load($field['settings']['target_type'], $target_ids);
  }
  else {
    $target_entities = array();
  }

  // Iterate through the fieldable entities again to attach the loaded data.
  foreach ($entities as $id => $entity) {
    $rekey = FALSE;

    foreach ($items[$id] as $delta => $item) {
      // Check whether the referenced entity could be loaded.
      if (is_array($item) && isset($item['target_id']) && isset($target_entities[$item['target_id']])) {
        // Replace the instance value with the term data.
        $items[$id][$delta]['entity'] = $target_entities[$item['target_id']];
        // Check whether the user has access to the referenced entity.
        $has_view_access = (entity_access('view', $field['settings']['target_type'], $target_entities[$item['target_id']]) !== FALSE);
        $has_update_access = (entity_access('update', $field['settings']['target_type'], $target_entities[$item['target_id']]) !== FALSE);
        $items[$id][$delta]['access'] = ($has_view_access || $has_update_access);
      }
      // We allow the full object to be passed through.
      elseif (is_object($item)) {
        list($entity_id, , $bundle) = entity_extract_ids($field['settings']['target_type'], $item);
        // Do nothing.
        $items[$id][$delta] = array();
        $items[$id][$delta]['target_id'] = $entity_id;
        $items[$id][$delta]['entity'] = $item;
        // Check whether the user has access to the referenced entity.
        $has_view_access = (entity_access('view', $field['settings']['target_type'], $item) !== FALSE);
        $has_update_access = (entity_access('update', $field['settings']['target_type'], $item) !== FALSE);
        $items[$id][$delta]['access'] = ($has_view_access || $has_update_access);
      }
      // Otherwise, unset the instance value, since the entity does not exist.
      else {
        unset($items[$id][$delta]);
        $rekey = TRUE;
      }
    }

    if ($rekey) {
      // Rekey the items array.
      $items[$id] = array_values($items[$id]);
    }
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function asset_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $result = array();
  $settings = $display['settings'];

  // Rebuild the items list to contain only those with access.
  foreach ($items as $key => $item) {
    if (empty($item['access'])) {
      unset($items[$key]);
    }
  }

  switch ($display['type']) {
    case 'asset_entity_view':
      foreach ($items as $delta => $item) {
        // Protect ourselves from recursive rendering.
        static $depth = 0;
        $depth++;
        if ($depth > 20) {
          throw new EntityReferenceRecursiveRenderingException(t('Recursive rendering detected when rendering entity @entity_type(@entity_id). Aborting rendering.', array('@entity_type' => $entity_type, '@entity_id' => $item['target_id'])));
        }

        $entity = clone $item['entity'];
        unset($entity->content);
        $result[$delta] = entity_view($field['settings']['target_type'], array($item['target_id'] => $entity), $settings['view_mode'], $langcode, FALSE);

        $depth = 0;
      }
      break;
  }

  return $result;
}
