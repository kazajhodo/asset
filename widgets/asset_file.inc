<?php

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_file_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $settings = $display['settings'];
  $element = array();
  foreach ($items as $delta => $item) {
    if($entity_type == 'asset'){
      $item['description'] = $entity->title;
    }
    $element[$delta] = array(
      '#theme' => 'file_link',
      '#file' => (object) $item,
    );
  }
  return $element;
}
