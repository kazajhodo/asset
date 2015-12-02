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
  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_gallery_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $element = array();
  $settings = $display['settings'];
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'image')) && isset($fields[$settings['field_name']])){
    $target = $fields[$settings['field_name']];
    $target_entity_wrappers = asset_entityreference_get($entity_type, $entity, $target);

    $element[0] = array(
      '#prefix' => '<div class="asset-gallery">',
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

    foreach ($items as $delta => $item) {
      if(isset($target_entity_wrappers[$delta])){
        $target_entity_wrapper = $target_entity_wrappers[$delta];
        $target_entity_type = $target_entity_wrapper->type();
        $target_entity = $target_entity_wrapper->value();
        if(!$target_entity) continue;
        $target_items = field_get_items($target_entity_type, $target_entity, $target['field_name']);
        foreach($target_items as $target_delta => $target_item){
          if($delta === 0){
            $element[0][0][$delta] = array(
              '#theme' => 'image_formatter',
              '#item' => $target_item,
              '#image_style' => $display['settings']['image_style'],
              '#path' => '',
            );
          }
          if(isset($element[0][1])){
            $element[0][1][$delta] = array(
              '#theme' => 'image_formatter',
              '#item' => $target_item,
              '#image_style' => $display['settings']['image_thumb_style'],
              '#path' => array(
                'path' => image_style_url($display['settings']['image_style'], $target_item['uri']),
              ),
            );
          }
        }
      }
    }
  }
  return $element;
}
