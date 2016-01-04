<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_file_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  $element['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('If empty, the entity title will be used.'),
    '#default_value' => $settings['title'],
  );
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_file_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();
  $summary[] = t('Title: !title', array('!title' => ($settings['title'] ? $settings['title'] : 'Entity title')));
  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_file_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $settings = $display['settings'];
  $element = array();
  foreach ($items as $delta => $item) {
    if($settings['title']){
      $item['description'] = $settings['title'];
    }
    elseif($entity_type == 'asset'){
      $item['description'] = $entity->title;
    }
    $element[$delta] = array(
      '#theme' => 'file_link',
      '#file' => (object) $item,
    );
  }
  return $element;
}
