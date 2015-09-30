<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_file_svg_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  $tags = array(
    'figure',
    'div',
    'section'
  );
  $element['wrapper_tag'] = array(
    '#type' => 'select',
    '#title' =>  t("Wrapper tag"),
    '#options' => drupal_map_assoc($tags),
    '#default_value' => $settings['wrapper_tag']
  );
  $element['classes'] = array(
    '#type' => 'textfield',
    '#title' =>  t("Extra wrapper tag class names"),
    '#size' => 32,
    '#default_value' => trim($settings['classes'])
  );
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_file_svg_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();
  if (isset($settings['wrapper_tag'])) {
    $summary[] = t("Wrapper tag") . ": " . $settings['wrapper_tag'];
  }
  if (is_string($settings['classes'])) {
    $settings['classes'] = trim($settings['classes']);
    if (!empty($settings['classes'])) {
      $summary[] = t("Extra class names") . ": " . $settings['classes'];
    }
  }
  if (isset($settings['add_script'])) {
    $settings['add_script'] = (bool) $settings['add_script'];
    if ($settings['add_script']) {
      $summary[] = t("PNG replacement script added");
    }
  }
  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_file_svg_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $settings = $display['settings'];
  $element = array();
  if (!empty($items) && is_array($items)) {
    foreach ($items as $index => $item) {
      $path = drupal_realpath($item['uri']);
      if (file_exists($path)) {
        $xml = file_get_contents($path);
        $settings = $display['settings'];
        $wrapper = !empty($settings['wrapper_tag'])? $settings['wrapper_tag'] : 'figure';
        $classes = array('svg-object');
        if (is_string($settings['classes'])) {
          if (!empty($settings['classes'])) {
            $classes = array_merge($classes, explode(' ', $settings['classes']));
          }
        }
        $attrs = array(
          'class' => $classes
        );
        $render_map = array(
          '#markup' => '<' . $wrapper . drupal_attributes($attrs).'>' . $xml . '</'. $wrapper .'>',
        );
        $element[] = $render_map;
      }
    }
  }
  return $element;
}
