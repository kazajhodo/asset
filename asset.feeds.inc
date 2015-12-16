<?php

/**
 * @file
 * Feeds hooks.
 */

/**
 * Implements hook_feeds_plugins().
 */
function asset_feeds_plugins() {
  $path = drupal_get_path('module', 'asset') . '/includes';

  $info = array();

  $type = 'asset';
  $entity_info = entity_get_info($type);

  $info['FeedsAssetProcessor'] = array(
    'name' => $entity_info['label'] . ' processor',
    'description' => 'Create and update ' . drupal_strtolower($entity_info['label']) . 's.',
    'help' => 'Create and update ' . drupal_strtolower($entity_info['label']) . 's from parsed content.',
    'plugin_key' => 'FeedsEntityProcessor',
    'handler' => array(
      'parent' => 'FeedsProcessor',
      'class' => 'FeedsAssetProcessor',
      'file' => 'asset.feeds.inc',
      'path' => $path,
    ),
   'type' => $type,
  );

  return $info;
}
