<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_gallery_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  if($fields = asset_entityreference_contains($field, 'image')){
    $field_names = array();
    foreach($fields as $key => $target){
      $target['parents'][] = $target['field_name'];
      $field_names[$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    $element['field_name'] = array(
      '#title' => t('Target image field'),
      '#type' => 'select',
      '#default_value' => $settings['field_name'],
      '#required' => TRUE,
      '#options' => $field_names,
      '#weight' => -100,
    );
  }
  if($fields = asset_entityreference_contains($field, 'video_embed_field')){
    $field_names = array();
    foreach($fields as $key => $target){
      $target['parents'][] = $target['field_name'];
      $field_names[$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    $element['field_video_name'] = array(
      '#title' => t('Target video field'),
      '#type' => 'select',
      '#default_value' => $settings['field_name'],
      '#required' => TRUE,
      '#options' => $field_names,
      '#weight' => -99,
    );
    $video_styles = video_embed_field_video_style_options(FALSE);
    $element['video_style'] = array(
      '#title' => t('Video style'),
      '#type' => 'select',
      '#default_value' => $settings['video_style'],
      '#options' => $video_styles,
      '#weight' => 100,
    );
  }
  if(!empty($element)){
    $image_styles = image_style_options(FALSE, PASS_THROUGH);
    $element['image_style'] = array(
      '#title' => t('Image style'),
      '#type' => 'select',
      '#default_value' => $settings['image_style'],
      '#empty_option' => t('None (original image)'),
      '#options' => $image_styles,
    );
    $element['image_thumb_style'] = array(
      '#title' => t('Image thumb style'),
      '#type' => 'select',
      '#default_value' => $settings['image_thumb_style'],
      '#empty_option' => t('None (original image)'),
      '#options' => $image_styles,
    );
  }
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_gallery_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();

  $label = 'NONE';
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'image')) && isset($fields[$settings['field_name']])){
    $target = $fields[$settings['field_name']];
    $target['parents'][] = $target['field_name'];
    $label = implode($target['parents'], ' > ');
  }
  $summary[] = t('Target field: @field', array('@field' => $label));
  if(($settings['field_video_name'] && $fields = asset_entityreference_contains($field, 'video_embed_field')) && isset($fields[$settings['field_video_name']])){
    $target = $fields[$settings['field_video_name']];
    $target['parents'][] = $target['field_name'];
    $label = implode($target['parents'], ' > ');
    $summary[] = t('Target video field: @field', array('@field' => $label));
  }
  $image_styles = image_style_options(FALSE, PASS_THROUGH);
  unset($image_styles['']);
  if (isset($image_styles[$settings['image_style']])) {
    $summary[] = t('Image style: @style', array('@style' => $image_styles[$settings['image_style']]));
  }
  else {
    $summary[] = t('Original image');
  }
  if (isset($image_styles[$settings['image_thumb_style']])) {
    $summary[] = t('Image thumb style: @style', array('@style' => $image_styles[$settings['image_thumb_style']]));
  }
  else {
    $summary[] = t('Original image');
  }
  if($settings['video_style']){
    $video_styles = video_embed_field_video_style_options(FALSE);
    // Styles could be lost because of enabled/disabled modules that defines
    // their styles in code.
    if (isset($video_styles[$settings['video_style']])) {
      $summary[] = t('Video style: @style', array('@style' => $video_styles[$settings['video_style']]));
    }
  }
  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_gallery_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $element = array();
  $settings = $display['settings'];
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'image')) && isset($fields[$settings['field_name']])){
    $image_target = $fields[$settings['field_name']];
    $target_image_entity_wrappers = asset_entityreference_get($entity_type, $entity, $image_target);
  }

  if(($settings['field_video_name'] && $video_fields = asset_entityreference_contains($field, 'video_embed_field')) && isset($video_fields[$settings['field_video_name']])){
    $video_target = $video_fields[$settings['field_video_name']];
    $target_video_entity_wrappers = asset_entityreference_get($entity_type, $entity, $video_target);
  }

  if(!empty($target_image_entity_wrappers) || !empty($target_video_entity_wrappers)){
    $gallery_id = drupal_html_class($instance['entity_type'] . '-' . $instance['bundle'] . '-' . $instance['field_name']);

    $element[0] = array(
      '#prefix' => '<div class="asset-gallery" data-asset-gallery="'.$gallery_id.'">',
      '#suffix' => '</div>',
      '#attached' => array(
        'js' => array(
          drupal_get_path('module', 'asset') . '/js/asset.widget.gallery.js',
        ),
      ),
    );

    $element[0][0] = array(
      '#prefix' => '<div class="asset-gallery-full">',
      '#suffix' => '</div>',
    );

    if(count($items) > 1){
      $element[0][1] = array(
        '#prefix' => '<div class="asset-gallery-thumbs">',
        '#suffix' => '</div>',
      );
    }

    $js_render = array();

    foreach ($items as $delta => $item) {

      // Images
      if(isset($target_image_entity_wrappers[$delta])){
        $target_entity_wrapper = $target_image_entity_wrappers[$delta];
        $target_entity_type = $target_entity_wrapper->type();
        $target_entity = $target_entity_wrapper->value();
        if(!$target_entity) continue;
        $target_items = field_get_items($target_entity_type, $target_entity, $image_target['field_name']);
        foreach($target_items as $target_delta => $target_item){
          $full = array(
            '#theme' => 'image_formatter',
            '#item' => $target_item,
            '#image_style' => $display['settings']['image_style'],
            '#path' => '',
            '#prefix' => '<div>',
            '#suffix' => '</div>',
          );
          if($delta === 0){
            $element[0][0][$delta] = $full;
          }
          $js_render[$delta] = drupal_render($full);
          if(isset($element[0][1])){
            $element[0][1][$delta] = array(
              '#theme' => 'image_formatter',
              '#item' => $target_item,
              '#image_style' => $display['settings']['image_thumb_style'],
              '#path' => array(
                'path' => image_style_url($display['settings']['image_style'], $target_item['uri']),
                'options' => array('attributes' => array('data-asset-gallery-id' => $delta, 'class' => array('image'))),
              ),
            );
          }
        }
      }

      // Videos
      if(isset($target_video_entity_wrappers[$delta])){
        $target_field = $video_target['field'];
        $target_instance = $video_target['instance'];
        $target_entity_wrapper = $target_video_entity_wrappers[$delta];
        $target_entity_type = $target_entity_wrapper->type();
        $target_entity = $target_entity_wrapper->value();
        if(!$target_entity) continue;
        $target_items = field_get_items($target_entity_type, $target_entity, $video_target['field_name']);
        foreach($target_items as $target_delta => $target_item){
          if ($ret = parse_url($target_item['video_url'])) {
            if (!isset($ret["scheme"])) {
              $target_item['video_url'] = "http://{$target_item['video_url']}";
            }
          }
          if(module_exists('colorbox')){
            $full = array(
              '#theme' => 'video_embed_field_colorbox_code',
              '#image_url' => $target_item['thumbnail_path'],
              '#image_style' => $display['settings']['image_style'],
              '#image_alt' => '',
              '#video_url' => $target_item['video_url'],
              '#video_style' => $display['settings']['video_style'],
              '#video_data' => unserialize($target_item['video_data']),
              '#prefix' => '<div>',
              '#suffix' => '</div>',
            );
          }else{
            $full = array(
              '#theme' => 'image_formatter',
              '#item' => array('uri' => $target_item['thumbnail_path']),
              '#image_style' => $display['settings']['image_style'],
              '#path' => array(
                'path' => $target_item['video_url'],
              ),
              '#prefix' => '<div>',
              '#suffix' => '</div>',
            );
          }
          if($delta === 0){
            $element[0][0][$delta] = $full;
          }
          $js_render[$delta] = drupal_render($full);
          if(isset($element[0][1])){
            $element[0][1][$delta] = array(
              '#theme' => 'image_formatter',
              '#item' => array('uri' => $target_item['thumbnail_path']),
              '#image_style' => $display['settings']['image_thumb_style'],
              '#path' => array(
                'path' => image_style_url($display['settings']['image_style'], $target_item['thumbnail_path']),
                'options' => array('attributes' => array('data-asset-gallery-id' => $delta, 'class' => array('video'))),
              ),
            );
          }
        }
      }
    }
    drupal_add_js(array('asset' => array('gallery' => array($gallery_id => $js_render))), 'setting');
  }
  return $element;
}
