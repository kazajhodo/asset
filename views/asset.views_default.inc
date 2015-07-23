<?php

/**
 * Implements hook_views_default_views().
 */
function asset_views_default_views() {
  $views = array();
  $views['asset_admin'] = asset_views_default_views_admin_list();

  $asset_types = asset_get_types();
  foreach($asset_types as $bundle => $asset_type){
    $views['asset_'.$asset_type->type] = asset_views_default_views_browser($asset_type);
  }
  return $views;
}

/**
 * Asset browser list.
 */
function asset_views_default_views_browser($asset_type){

  $info = field_info_instances('asset', $asset_type->type);
  $has_image = $has_file = $has_video = $has_reference = FALSE;
  foreach($info as $instance){
    $field = field_info_field($instance['field_name']);
    switch($field['type']){
      case 'image':
        $has_image = $field['field_name'];
        break;
      case 'file':
        $has_file = $field['field_name'];
        break;
      case 'video_embed_field':
        $has_video = $field['field_name'];
        break;
      case 'entityreference':
        $has_reference = array(
          'field_name' => $field['field_name'],
          'instance' => $instance,
          'field' => $field,
        );
        break;
    }
  }

  $view = new view();
  $view->name = 'asset_' . $asset_type->type;
  $view->description = 'A view used to list all asset types of "'.$asset_type->name.'".';
  $view->tag = 'assets';
  $view->base_table = 'asset';
  $view->human_name = 'ASSETS | ' . $asset_type->name;
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['hide_admin_links'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '24';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = 'No Results Found';
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = '<div class="alert-box status">No '.$asset_type->plural.' found.</div>';
  /* Field: Asset: Label */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'asset';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = '';
  $handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
  /* Field: Asset: Image */
  if($has_image){
    $handler->display->display_options['fields'][$has_image]['id'] = $has_image;
    $handler->display->display_options['fields'][$has_image]['table'] = 'field_data_' . $has_image;
    $handler->display->display_options['fields'][$has_image]['field'] = $has_image;
    $handler->display->display_options['fields'][$has_image]['label'] = '';
    $handler->display->display_options['fields'][$has_image]['element_label_colon'] = FALSE;
    $handler->display->display_options['fields'][$has_image]['click_sort_column'] = 'fid';
    $handler->display->display_options['fields'][$has_image]['settings'] = array(
      'image_style' => 'asset-thumb',
      'image_link' => '',
    );
  }
  /* Field: Asset: Video */
  elseif($has_video){
    $handler->display->display_options['fields'][$has_video]['id'] = $has_video;
    $handler->display->display_options['fields'][$has_video]['table'] = 'field_data_' . $has_video;
    $handler->display->display_options['fields'][$has_video]['field'] = $has_video;
    $handler->display->display_options['fields'][$has_video]['label'] = '';
    $handler->display->display_options['fields'][$has_video]['click_sort_column'] = 'video_url';
    $handler->display->display_options['fields'][$has_video]['type'] = 'video_embed_field_thumbnail';
    $handler->display->display_options['fields'][$has_video]['settings'] = array(
      'image_style' => 'asset-thumb',
      'image_link' => '',
    );
  }
  /* Field: Asset: File */
  elseif($has_file){
    $handler->display->display_options['fields'][$has_file]['id'] = $has_file;
    $handler->display->display_options['fields'][$has_file]['table'] = 'field_data_' . $has_file;
    $handler->display->display_options['fields'][$has_file]['field'] = $has_file;
    $handler->display->display_options['fields'][$has_file]['label'] = '';
    $handler->display->display_options['fields'][$has_file]['element_label_colon'] = FALSE;
    $handler->display->display_options['fields'][$has_file]['click_sort_column'] = 'fid';
    $handler->display->display_options['fields'][$has_file]['type'] = 'file_mime_type_icon';
  }
  elseif($has_reference &&
    isset($has_reference['field']['settings']['handler_settings']['target_bundles']) &&
    count($has_reference['field']['settings']['handler_settings']['target_bundles']) == 1){

    if($fields = asset_entityreference_contains($has_reference['field'], 'image')){
      foreach($fields as $key => $data){
        $handler->display->display_options['fields'][$has_reference['field_name']]['id'] = $has_reference['field_name'];
        $handler->display->display_options['fields'][$has_reference['field_name']]['table'] = 'field_data_' . $has_reference['field_name'];
        $handler->display->display_options['fields'][$has_reference['field_name']]['field'] = $has_reference['field_name'];
        $handler->display->display_options['fields'][$has_reference['field_name']]['label'] = '';
        // $handler->display->display_options['fields'][$has_reference['field_name']]['click_sort_column'] = 'target_id';
        $handler->display->display_options['fields'][$has_reference['field_name']]['type'] = 'asset_image';
        $handler->display->display_options['fields'][$has_reference['field_name']]['settings'] = array(
          'field_name' => $key,
          'image_style' => 'asset-thumb',
          'image_link' => '',
        );
        break;
      }
    }
  }
  /* Field: Asset: Asset Select */
  $handler->display->display_options['fields']['asset_select']['id'] = 'asset_select';
  $handler->display->display_options['fields']['asset_select']['table'] = 'asset';
  $handler->display->display_options['fields']['asset_select']['field'] = 'asset_select';
  $handler->display->display_options['fields']['asset_select']['label'] = '';
  $handler->display->display_options['fields']['asset_select']['element_label_colon'] = FALSE;
  /* Sort criterion: Asset: Created */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'asset';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Asset: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'asset';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    $asset_type->type => $asset_type->type,
  );
  /* Filter criterion: Asset: Label */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'asset';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Name';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';

  return $view;

}



/**
 * Asset admin list.
 *
 */
function asset_views_default_views_admin_list() {

  $view = new view();
  $view->name = 'assets';
  $view->description = 'A list of all assets';
  $view->tag = 'assets';
  $view->base_table = 'asset';
  $view->human_name = 'ASSETS | Admin List';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Assets';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'create any asset type';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'asset_icon' => 'asset_icon',
    'name' => 'name',
    'asset_link' => 'name',
    'asset_ops' => 'asset_ops',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'asset_icon' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '&nbsp;&nbsp;',
      'empty_column' => 0,
    ),
    'asset_link' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'asset_ops' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = '<div class="alert-box">No assets have been created yet</div>';
  /* Relationship: Asset: Author */
  $handler->display->display_options['relationships']['uid']['id'] = 'uid';
  $handler->display->display_options['relationships']['uid']['table'] = 'asset';
  $handler->display->display_options['relationships']['uid']['field'] = 'uid';
  /* Field: Asset: Icon */
  $handler->display->display_options['fields']['asset_icon']['id'] = 'asset_icon';
  $handler->display->display_options['fields']['asset_icon']['table'] = 'asset';
  $handler->display->display_options['fields']['asset_icon']['field'] = 'asset_icon';
  /* Field: Asset: Label */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'asset';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Name';
  /* Field: Asset: Usage */
  $handler->display->display_options['fields']['asset_usage']['id'] = 'asset_usage';
  $handler->display->display_options['fields']['asset_usage']['table'] = 'asset';
  $handler->display->display_options['fields']['asset_usage']['field'] = 'asset_usage';
  /* Field: Asset: Link */
  $handler->display->display_options['fields']['asset_link']['id'] = 'asset_link';
  $handler->display->display_options['fields']['asset_link']['table'] = 'asset';
  $handler->display->display_options['fields']['asset_link']['field'] = 'asset_link';
  $handler->display->display_options['fields']['asset_link']['label'] = 'View';
  /* Field: User: Name */
  $handler->display->display_options['fields']['name_1']['id'] = 'name_1';
  $handler->display->display_options['fields']['name_1']['table'] = 'users';
  $handler->display->display_options['fields']['name_1']['field'] = 'name';
  $handler->display->display_options['fields']['name_1']['relationship'] = 'uid';
  $handler->display->display_options['fields']['name_1']['label'] = 'Author';
  /* Field: Asset: Created */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'asset';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['date_format'] = 'short';
  $handler->display->display_options['fields']['created']['second_date_format'] = 'long';
  /* Field: Asset: Changed */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'asset';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['date_format'] = 'short';
  $handler->display->display_options['fields']['changed']['second_date_format'] = 'long';
  /* Field: Asset: Operations links */
  $handler->display->display_options['fields']['asset_ops']['id'] = 'asset_ops';
  $handler->display->display_options['fields']['asset_ops']['table'] = 'asset';
  $handler->display->display_options['fields']['asset_ops']['field'] = 'asset_ops';
  $handler->display->display_options['fields']['asset_ops']['label'] = 'Operations';
  /* Sort criterion: Asset: Changed */
  $handler->display->display_options['sorts']['changed']['id'] = 'changed';
  $handler->display->display_options['sorts']['changed']['table'] = 'asset';
  $handler->display->display_options['sorts']['changed']['field'] = 'changed';
  $handler->display->display_options['sorts']['changed']['order'] = 'DESC';
  /* Filter criterion: Asset: Label */
  $handler->display->display_options['filters']['name']['id'] = 'name';
  $handler->display->display_options['filters']['name']['table'] = 'asset';
  $handler->display->display_options['filters']['name']['field'] = 'name';
  $handler->display->display_options['filters']['name']['operator'] = 'contains';
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Label';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Asset: Type */
  $handler->display->display_options['filters']['type_1']['id'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['table'] = 'asset';
  $handler->display->display_options['filters']['type_1']['field'] = 'type';
  $handler->display->display_options['filters']['type_1']['group'] = 1;
  $handler->display->display_options['filters']['type_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['operator_id'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type_1']['expose']['operator'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['identifier'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'assets_admin_page');
  $handler->display->display_options['path'] = 'admin/content/assets/list';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'List';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['tab_options']['type'] = 'tab';
  $handler->display->display_options['tab_options']['title'] = 'Assets';
  $handler->display->display_options['tab_options']['description'] = 'Manage assets';
  $handler->display->display_options['tab_options']['weight'] = '0';
  $handler->display->display_options['tab_options']['name'] = 'management';

  return $view;
}
