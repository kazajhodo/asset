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
    $element['as_url'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display as URL'),
      '#default_value' => $settings['as_url'],
      '#id' => $elid,
    );
    $element['first_only'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display first image only'),
      '#default_value' => $settings['first_only'],
      '#id' => $elid . '-first',
    );
    $element['first_only']['#states'] = array(
      'visible' => array(
        '#' . $elid . '-random' => array('checked' => FALSE),
      ),
    );
    $element['random'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display random image'),
      '#default_value' => $settings['random'],
      '#id' => $elid . '-random',
    );
    $element['random']['#states'] = array(
      'visible' => array(
        '#' . $elid . '-first' => array('checked' => FALSE),
      ),
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
  if($settings['first_only']){
    $summary .= '<br>' . t('First: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
  }
  if($settings['random']){
    $summary .= '<br>' . t('Random: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
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
    if($settings['first_only']){
      $items = array(reset($items));
    }
    if($settings['random']){
      $rand = rand(0, count($items) - 1);
      $items = array($rand => $items[$rand]);
    }
    $target_entity_wrappers = asset_entityreference_get($entity_type, $entity, $target);
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
        if ($display['settings']['image_link'] == 'content') {
          foreach($element[$delta] as &$e){
            if(isset($e['#path']['path'])){
              $e['#path'] = entity_uri($entity_type, $entity);
            }
          }
        }
      }
    }
  }
  return $element;
}
