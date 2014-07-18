<?php

/**
 * Implements hook_views_data_alter()
 */
function asset_views_data_alter(&$data) {
  $data['asset']['asset_link'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a link to the asset.'),
      'handler' => 'asset_handler_link_field',
    ),
  );
  if(module_exists('fawesome')){
    $data['asset']['asset_icon'] = array(
      'field' => array(
        'title' => t('Icon'),
        'help' => t('Provide the type icon to the asset.'),
        'handler' => 'asset_handler_icon_field',
      ),
    );
  }
  $data['asset']['asset_edit'] = array(
    'field' => array(
      'title' => t('Edit Link'),
      'help' => t('Provide a link to the edit form for the asset.'),
      'handler' => 'asset_handler_edit_field',
    ),
  );
  $data['asset']['asset_delete'] = array(
    'field' => array(
      'title' => t('Delete Link'),
      'help' => t('Provide a link to delete the asset.'),
      'handler' => 'asset_handler_delete_field',
    ),
  );
  $data['asset']['asset_ops'] = array(
    'field' => array(
      'title' => t('Operations links'),
      'help' => t('Display all operations available for this asset.'),
      'handler' => 'asset_handler_ops_field',
    ),
  );
  $data['asset']['asset_usage'] = array(
    'field' => array(
      'title' => t('Usage'),
      'help' => t('Display the usage count for an asset.'),
      'handler' => 'asset_handler_usage_field',
    ),
  );
  $data['asset']['asset_select'] = array(
    'field' => array(
      'title' => t('Asset Select'),
      'help' => t('Provide a special link to the asset which is utilized by asset modules.'),
      'handler' => 'asset_handler_select_field',
    ),
  );

  $data['asset']['created']['field']['handler'] = 'views_handler_field_date';
  $data['asset']['changed']['field']['handler'] = 'views_handler_field_date';

  $data['asset']['uid'] = array(
    'title' => t('Author uid'),
    'help' => t('The user authoring the content. If you need more fields than the uid add the content: author relationship'),
    'relationship' => array(
      'title' => t('Author'),
      'help' => t('Relate asset to the user who created it.'),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => 'author',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'field' => array(
      'handler' => 'views_handler_field_user',
    ),
  );

  $data['asset']['type'] = array(
    'title' => t('Type'),
    'help' => t('The asset type (for example, "image", "video", etc).'),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'asset_handler_type_filter',
    ),
    'argument' => array(
      'handler' => 'asset_handler_type_argument',
    ),
    'field' => array(
      'handler' => 'asset_handler_type_field',
      'click sortable' => TRUE
    ),
  );
}
