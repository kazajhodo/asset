<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_file_icon_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  $element['show_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show title'),
    '#default_value' => $settings['show_title'],
  );
  $element['show_download'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show download text'),
    '#default_value' => $settings['show_download'],
  );
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_file_icon_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();

  $yes = '<i class="fa fa-check text-success"></i>';
  $no  = '<i class="fa fa-times text-warning"></i>';

  $summary[] = t('Show title: !title', array('!title' => ($settings['show_title'] ? $yes : $no)));
  $summary[] = t('Show download text: !download', array('!download' => ($settings['show_download'] ? $yes : $no)));
  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_file_icon_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $settings = $display['settings'];
  $element = array();
  foreach ($items as $delta => $item) {
    if($entity_type == 'asset'){
      $item['description'] = $entity->title;
    }
    $element[$delta] = array(
      '#theme' => 'asset_file_link',
      '#file' => (object) $item,
      '#show_download' => $settings['show_download'],
      '#show_title' => $settings['show_title'],
    );
  }
  return $element;
}
