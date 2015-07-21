<?php
/**
 * @file
 * Provides the Views integration for views content cache.
 */

/**
 * Implements hook_views_plugins().
 */
function views_asset_cache_views_plugins() {
  return array(
    'cache' => array(
      'views_asset_cache' => array(
        'path' => drupal_get_path('module', 'views_asset_cache') . '/views',
        'title' => t('Asset-based'),
        'help' => t('Advanced asset based caching options for views.'),
        'handler' => 'views_asset_cache_plugin_cache',
        'uses options' => TRUE,
      ),
    ),
  );
}
