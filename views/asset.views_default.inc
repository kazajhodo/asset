<?php

/**
 * Implements hook_views_default_views().
 */
function asset_views_default_views() {
  $views = array();
  $views['asset_admin'] = asset_views_default_asset_library();
  $views['asset_browser'] = asset_views_default_asset_browser();
  return $views;
}

/**
 * Asset browser default view.
 */
function asset_views_default_asset_browser() {

  // Common Elements
  $field_title = array();
  $field_title['title']['id'] = 'title';
  $field_title['title']['table'] = 'asset';
  $field_title['title']['field'] = 'title';
  $field_title['title']['label'] = '';
  $field_title['title']['element_label_colon'] = FALSE;

  $field_asset_select = array();
  $field_asset_select['asset_select']['id'] = 'asset_select';
  $field_asset_select['asset_select']['table'] = 'asset';
  $field_asset_select['asset_select']['field'] = 'asset_select';
  $field_asset_select['asset_select']['label'] = '';
  $field_asset_select['asset_select']['element_label_colon'] = FALSE;

  $filter_asset_type = array();
  $filter_asset_type['type']['id'] = 'type';
  $filter_asset_type['type']['table'] = 'asset';
  $filter_asset_type['type']['field'] = 'type';
  $filter_asset_type['type']['group'] = 1;
  $filter_asset_type['type']['value'] = array();

  $filter_title = array();
  $filter_title['title']['id'] = 'title';
  $filter_title['title']['table'] = 'asset';
  $filter_title['title']['field'] = 'title';
  $filter_title['title']['operator'] = 'contains';
  $filter_title['title']['group'] = 1;
  $filter_title['title']['exposed'] = TRUE;
  $filter_title['title']['expose']['operator_id'] = 'name_op';
  $filter_title['title']['expose']['label'] = 'Name';
  $filter_title['title']['expose']['operator'] = 'name_op';
  $filter_title['title']['expose']['identifier'] = 'title';



  $view = new view();
  $view->name = 'asset_browser';
  $view->description = 'A view used to list all asset types of "File".';
  $view->tag = 'assets';
  $view->base_table = 'asset';
  $view->human_name = 'Asset Browser';
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
  $handler->display->display_options['style_options']['row_class'] = 'asset-browser-items';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = 'No Results Found';
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = '<div class="alert-box messages status">No Files found.</div>';

  $handler->display->display_options['fields'] = array();
  /* Field: Asset: Title */
  $handler->display->display_options['fields'] += $field_title;
  /* Field: Asset: Asset Select */
  $handler->display->display_options['fields'] += $field_asset_select;

  /* Sort criterion: Asset: Created */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'asset';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';

  $handler->display->display_options['filters'] = array();
  /* Filter criterion: Asset: Type */
  // $handler->display->display_options['filters'] += $filter_asset_type;
  /* Filter criterion: Asset: Title */
  $handler->display->display_options['filters'] += $filter_title;

  foreach(asset_get_types() as $bundle => $asset_type){
    $has_field = array();
    foreach(field_info_instances('asset', $asset_type->type) as $instance){
      $field = field_info_field($instance['field_name']);
      $has_field[$field['type']][] = array(
        $field['field_name'],
        $field,
        $instance,
      );
    }

    $handler = $view->new_display('embed', $asset_type->title, $asset_type->type);
    $handler->display->display_options['defaults']['fields'] = FALSE;

    $handler->display->display_options['fields'] = array();
    /* Field: Asset: Title */
    $handler->display->display_options['fields'] += $field_title;

    if(isset($has_field['image'])){
      /* Field: Asset: Image */
      list($field_name, $field, $instance) = reset($has_field['image']);
      $handler->display->display_options['fields'][$field_name]['id'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['table'] = 'field_data_' . $field_name;
      $handler->display->display_options['fields'][$field_name]['field'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['label'] = '';
      $handler->display->display_options['fields'][$field_name]['element_label_colon'] = FALSE;
      $handler->display->display_options['fields'][$field_name]['click_sort_column'] = 'fid';
      $handler->display->display_options['fields'][$field_name]['settings'] = array(
        'image_style' => 'asset-thumb',
        'image_link' => '',
      );
    }

    elseif(isset($has_field['video_embed_field'])){
      /* Field: Asset: Video Embed */
      list($field_name, $field, $instance) = reset($has_field['video_embed_field']);
      $handler->display->display_options['fields'][$field_name]['id'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['table'] = 'field_data_' . $field_name;
      $handler->display->display_options['fields'][$field_name]['field'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['label'] = '';
      $handler->display->display_options['fields'][$field_name]['click_sort_column'] = 'video_url';
      $handler->display->display_options['fields'][$field_name]['type'] = 'video_embed_field_thumbnail';
      $handler->display->display_options['fields'][$field_name]['settings'] = array(
        'image_style' => 'asset-thumb',
        'image_link' => '',
      );
    }

    elseif(isset($has_field['file'])){
      /* Field: Asset: File */
      list($field_name, $field, $instance) = reset($has_field['file']);
      $handler->display->display_options['fields'][$field_name]['id'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['table'] = 'field_data_' . $field_name;
      $handler->display->display_options['fields'][$field_name]['field'] = $field_name;
      $handler->display->display_options['fields'][$field_name]['label'] = '';
      $handler->display->display_options['fields'][$field_name]['element_label_colon'] = FALSE;
      $handler->display->display_options['fields'][$field_name]['click_sort_column'] = 'fid';
      $handler->display->display_options['fields']['job_file']['type'] = 'asset_file_icon';
      $handler->display->display_options['fields']['job_file']['settings'] = array(
        'show_title' => 0,
        'show_download' => 0,
      );
    }

    elseif(isset($has_field['entityreference'])){
      /* Field: Children: Entity Reference */
      foreach($has_field['entityreference'] as $reference){
        $found = FALSE;
        list($field_name, $field, $instance) = $reference;
        if(isset($field['settings']['handler_settings']['target_bundles']) &&
          count($field['settings']['handler_settings']['target_bundles']) == 1){
          if($fields = asset_entityreference_contains($field, 'image')){
            foreach($fields as $key => $data){
              $found = TRUE;
              $handler->display->display_options['fields'][$field_name]['id'] = $field_name;
              $handler->display->display_options['fields'][$field_name]['table'] = 'field_data_' . $field_name;
              $handler->display->display_options['fields'][$field_name]['field'] = $field_name;
              $handler->display->display_options['fields'][$field_name]['label'] = '';
              $handler->display->display_options['fields'][$field_name]['type'] = 'asset_image';
              $handler->display->display_options['fields'][$field_name]['settings'] = array(
                'field_name' => $key,
                'image_style' => 'asset-thumb',
                'image_link' => '',
              );
              break;
            }
          }
        }
        if($found){
          break;
        }
      }
    }

    /* Field: Asset: Asset Select */
    $handler->display->display_options['fields'] += $field_asset_select;

    $handler->display->display_options['defaults']['filter_groups'] = FALSE;
    $handler->display->display_options['defaults']['filters'] = FALSE;

    $handler->display->display_options['filters'] = array();
    /* Filter criterion: Asset: Type */
    $handler->display->display_options['filters'] += $filter_asset_type;
    $handler->display->display_options['filters']['type']['value'] = array(
      $asset_type->type => $asset_type->type,
    );
    /* Filter criterion: Asset: Title */
    $handler->display->display_options['filters'] += $filter_title;
  }

  return $view;
}



/**
 * Asset admin list.
 *
 */
function asset_views_default_asset_library() {

  $view = new view();
  $view->name = 'assets';
  $view->description = 'A list of all assets';
  $view->tag = 'assets';
  $view->base_table = 'asset';
  $view->human_name = 'Asset Library';
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
    'title' => 'title',
    'asset_link' => 'title',
    'asset_ops' => 'asset_ops',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'asset_icon' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
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
  $handler->display->display_options['empty']['area_text_custom']['content'] = '<div class="alert-box messages">No assets have been created yet</div>';
  /* Relationship: Asset: Author */
  $handler->display->display_options['relationships']['uid']['id'] = 'uid';
  $handler->display->display_options['relationships']['uid']['table'] = 'asset';
  $handler->display->display_options['relationships']['uid']['field'] = 'uid';
  /* Field: Asset: Icon */
  $handler->display->display_options['fields']['asset_icon']['id'] = 'asset_icon';
  $handler->display->display_options['fields']['asset_icon']['table'] = 'asset';
  $handler->display->display_options['fields']['asset_icon']['field'] = 'asset_icon';
  /* Field: Asset: Label */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'asset';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
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
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'asset';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['group'] = 1;
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Label';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
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
