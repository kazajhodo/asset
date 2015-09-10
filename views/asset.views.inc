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
      'title' => t('Operation links'),
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

  $data['asset']['asset_creation'] = array(
    'title' => t('Asset Creation'),
    'help' => t('Shows asset creation links.'),
    'area' => array(
      'handler' => 'asset_handler_creation_field',
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

  // published status
  $data['asset']['status'] = array(
    'title' => t('Published'),
    'help' => t('Whether or not the content is published.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'published-notpublished' => array(t('Published'), t('Not published')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Published'),
      'type' => 'yes-no',
      'use equal' => TRUE, // Use status = 1 instead of status <> 0 in WHERE statment
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['asset']['render'] = array(
    'title' => t('Rendered Asset'),
    'help' => t('Displays a single chosen asset.'),
    'area' => array(
      'handler' => 'asset_handler_area_entity',
    ),
  );

  // Alter defaults provided by entity module.
  $data['asset']['title']['title'] = 'Title';
}

/**
 * Implements hook_views_plugins().
 */
function asset_views_plugins() {
  return array(
    'module' => 'views', // This just tells our themes are elsewhere.
    'argument validator' => array(
      'asset' => array(
        'title' => t('Asset'),
        'handler' => 'asset_plugin_argument_validate',
      ),
    ),
  );
}

/**
 * Implements hook_views_pre_view().
 */
function asset_views_pre_view(&$view, &$display_id, &$args){
  if($view->name == 'asset_browser' && !empty($args[0])){
    // Argument 0 determines whether we load javascript entity outputs for
    // the assets in the browser window.
    $view->asset_js_load = $args[0];
    $view->asset_view_mode = $args[1];
    $view->asset_preview = $args[2];
  }
}
