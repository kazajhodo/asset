<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_WIDGET_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();

  // Code Here

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_WIDGET_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();

  // Code Here

  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_WIDGET_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $settings = $display['settings'];
  $element = array();

  // Code Here

  return $element;
}
