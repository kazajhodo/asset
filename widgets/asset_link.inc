<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_link_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  $element['icon'] = array(
    '#type' => 'fawesome_select',
    '#title' => t('Icon'),
    '#default_value' => $settings['icon'],
  );
  $element['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#description' => t('If empty, the referenced entity title will be used.'),
    '#default_value' => $settings['label'],
  );
  if($fields = asset_entityreference_contains($field, 'file')){
    $field_names = array('- None -');
    foreach($fields as $key => $target){
      $target['parents'][] = $target['field_name'];
      $field_names[$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    $element['field_name'] = array(
      '#type' => 'select',
      '#title' => t('Link to'),
      '#title' => t('If empty, will link to entity url.'),
      '#default_value' => $settings['field_name'],
      '#required' => TRUE,
      '#options' => $field_names,
    );
  }
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_link_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();
  $summary[] = t('Icon: !icon', array('!icon' => ($settings['icon'] ? '<i class="fa fa-' . $settings['icon'] . '"></i>' : '<em>- None -</em>')));
  $summary[] = t('Label: @label', array('@label' => ($settings['label'] ? $settings['label'] : 'Entity title')));
  $label = 'Entity';
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'file')) && isset($fields[$settings['field_name']])){
    $target = $fields[$settings['field_name']];
    $target['parents'][] = $target['field_name'];
    $label = implode($target['parents'], ' > ');
  }
  $summary[] = t('Link to: @field', array('@field' => $label));
  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_link_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $element = array();
  $settings = $display['settings'];
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'file')) && isset($fields[$settings['field_name']])){
    $target = $fields[$settings['field_name']];
    $target_field = $target['field'];
    $target_instance = $target['instance'];
    $target_entity_wrappers = asset_entityreference_get($entity_type, $entity, $target);
  }
  foreach ($items as $delta => $item) {
    $markup = '';
    if($settings['icon']){
      $markup .= '<i class="fa fa-'.$settings['icon'].'"></i> ';
    }
    $label = $settings['label'];
    if(!$label){
      $wrapper = entity_metadata_wrapper($entity_type, $entity);
      $label = $wrapper->{$field['field_name']}->label();
    }
    if(isset($target_entity_wrappers[$delta])){
      $url = array('path'=>'','options'=>array());
      $target_entity_wrapper = $target_entity_wrappers[$delta];
      if($target_file = $target_entity_wrapper->{$target['field_name']}->value()){
        $url['path'] = file_create_url($target_file['uri']);
      }
    }
    else{
      $url = entity_uri($entity_type, $entity);
    }
    $markup .= l($label, $url['path'], $url['options']);
    $element[$delta] = array(
      '#markup' => $markup,
    );
  }
  return $element;
}
