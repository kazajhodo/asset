<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_image_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  if($fields = asset_entityreference_contains($field, 'image')){
    $field_names = array();
    foreach($fields as $key => $target){
      $target['parents'][] = $target['field_name'];
      $field_names[$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    $elid   = drupal_html_id('asset_image_as_url');
    module_load_include('inc', 'image', 'image.field');
    $element = image_field_formatter_settings_form($field, $instance, $view_mode, $form, $form_state);
    $element['field_name'] = array(
      '#title' => t('Target image field'),
      '#type' => 'select',
      '#default_value' => $settings['field_name'],
      '#required' => TRUE,
      '#options' => $field_names,
      '#weight' => -100,
    );
    $element['image_link']['#weight'] = 100;
    $element['image_link']['#states'] = array(
      'visible' => array(
        '#' . $elid => array('checked' => FALSE),
      ),
    );
    $links = asset_entityreference_contains($field, 'link_field');
    foreach($links as $key => $target){
      $target['parents'][] = $target['field_name'];
      $element['image_link']['#options'][$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    $element['as_url'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display as URL'),
      '#default_value' => $settings['as_url'],
      '#id' => $elid,
    );
    $element['limit'] = array(
      '#type' => 'textfield',
      '#title' => t('Limit the number of items to display'),
      '#default_value' => $settings['limit'],
      '#id' => $elid . '-limit',
    );
    $element['random'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display in random order'),
      '#default_value' => $settings['random'],
      '#id' => $elid . '-random',
    );
  }
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_image_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();
  module_load_include('inc', 'image', 'image.field');
  $label = 'NONE';
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'image')) && isset($fields[$settings['field_name']])){
    $target = $fields[$settings['field_name']];
    $target['parents'][] = $target['field_name'];
    $label = implode($target['parents'], ' > ');
  }
  $summary = t('Target field: @field', array('@field' => $label));
  $summary .= '<br>' . image_field_formatter_settings_summary($field, $instance, $view_mode);
  if($settings['as_url']){
    $summary .= '<br>' . t('As URL: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
  }
  if($settings['limit']){
    $summary .= '<br>' . t('Limit to: !limit', array('!limit' => format_plural($settings['limit'], '1 item', '@count items')));
  }
  if($settings['random']){
    $summary .= '<br>' . t('Random: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
  }
  if($settings['image_link'] && !in_array($settings['image_link'], array('content','file'))){
    $fields = asset_entityreference_contains($field, 'link_field');
    if(isset($fields[$settings['image_link']])){
      $target = $fields[$settings['image_link']];
      $target['parents'][] = $target['field_name'];
      $label = implode($target['parents'], ' > ');
      $summary .= '<br>' . t('Linked to: @field', array('@field' => $label));
    }
  }
  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_image_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $settings = $display['settings'];
  $element = array();
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'image')) && isset($fields[$settings['field_name']])){
    $target = $fields[$settings['field_name']];
    $target_field = $target['field'];
    $target_instance = $target['instance'];
    if($settings['random']){
      $keys = array_keys($items);
      shuffle($keys);
      $random = array();
      foreach ($keys as $key){
        $random[$key] = $items[$key];
      }
      $items = $random;
    }
    if($settings['limit']){
      $items = array_slice($items, 0, $settings['limit'], true);
    }
    $target_entity_wrappers = asset_entityreference_get($entity_type, $entity, $target);

    // Nested link field support
    if (!in_array($settings['image_link'], array('content','file'))) {
      $link_fields = asset_entityreference_contains($field, 'link_field');
      if(isset($link_fields[$settings['image_link']])){
        $link_target = $link_fields[$settings['image_link']];
        $link_target_entity_wrappers = asset_entityreference_get($entity_type, $entity, $link_target);
      }
    }

    foreach ($items as $delta => $item) {
      if(isset($target_entity_wrappers[$delta])){
        $target_entity_wrapper = $target_entity_wrappers[$delta];
        $target_entity_type = $target_entity_wrapper->type();
        $target_entity = $target_entity_wrapper->value();
        $target_items = field_get_items($target_entity_type, $target_entity, $target['field_name']);
        module_load_include('inc', 'image', 'image.field');
        $element[$delta] = image_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display);
        if($settings['as_url']){
          foreach($element[$delta] as &$e){
            $element[$delta] = array(
              '#markup' => !empty($e['#image_style']) ? image_style_url($e['#image_style'], $e['#item']['uri']) : file_create_url($e['#item']['uri'])
            );
          }
        }
        if ($settings['image_link'] == 'content') {
          foreach($element[$delta] as &$e){
            if(isset($e['#path']['path'])){
              $e['#path'] = entity_uri($entity_type, $entity);
            }
          }
        }
        // Nested link render
        elseif (isset($link_target_entity_wrappers)) {
          if(isset($link_target_entity_wrappers[$delta])){
            $link_target_entity_wrapper = $link_target_entity_wrappers[$delta];
            $link_target_entity_type = $link_target_entity_wrapper->type();
            $link_target_entity = $link_target_entity_wrapper->value();
            $link_target_items = field_get_items($link_target_entity_type, $link_target_entity, $link_target['field_name']);
            if(isset($link_target_items[0]['url'])){
              foreach($element[$delta] as $i => &$e){
                $e['#path']['path'] = url($link_target_items[$i]['url']);
                $e['#path']['options'] = $link_target_items[$i]['attributes'];
              }
            }
          }
        }
      }
    }
  }
  return $element;
}
