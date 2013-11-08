<?php

/**
 * @file
 * Providing extra functionality for the Asset UI via views.
 */


/**
 * Implements hook_views_data()
 */
function asset_views_data_alter(&$data) {

  // Browse fields
  $data['asset']['asset_select'] = array(
    'field' => array(
      'title' => t('Asset Select'),
      'help' => t('A selection element for asset browser.'),
      'handler' => 'asset_handler_asset_select_field',
    ),
  );

  // Index Fields
  $data['asset']['link_asset'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a link to the asset.'),
      'handler' => 'asset_handler_link_field',
    ),
  );
  $data['asset']['edit_asset'] = array(
    'field' => array(
      'title' => t('Edit Link'),
      'help' => t('Provide a link to the edit form for the asset.'),
      'handler' => 'asset_handler_edit_link_field',
    ),
  );
  $data['asset']['delete_asset'] = array(
    'field' => array(
      'title' => t('Delete Link'),
      'help' => t('Provide a link to delete the asset.'),
      'handler' => 'asset_handler_delete_link_field',
    ),
  );
  // This content of this field are decided based on the menu structure that
  // follows assets/asset/%id/op
  $data['asset']['operations'] = array(
    'field' => array(
      'title' => t('Operations links'),
      'help' => t('Display all operations available for this asset.'),
      'handler' => 'asset_handler_asset_operations_field',
    ),
  );
}
