<?php

function asset_nest_entity_form($form, &$form_state, $child_entity, $vars){
  // dsm($form_state);
  extract($vars);
  $form_state['asset_nest'] = $vars;
  // $form_state['asset_nest']['entity_type'] = $child_entity_type;
  // $form_state['asset_nest']['parent_entity_type'] = $parent_entity_type;
  // $form_state['asset_nest']['parent_entity'] = $parent_entity;
  // $form_state['asset_nest']['parent_field_name'] = $parent_field_name;
  // $form_state['asset_nest']['parent_form_id'] = $parent_form_id;

  form_load_include($form_state, 'inc', 'asset', 'includes/asset.admin');
  $form += asset_edit_form($form, $form_state, $child_entity);
  $wrapper = $form_state['asset_nest']['wrapper'] = 'asset-nest-form';
  $form['#id'] = $wrapper;
  $form['actions']['submit']['#submit'] = array('asset_nest_entity_form_submit');
  $form['#submit'][] = 'asset_nest_entity_form_submit';
  $form['actions']['submit']['#name'] = time();
  $form['actions']['submit']['#ajax'] = array(
    // 'callback' => 'asset_nest_entity_form_ajax_submit',
    'callback' => 'asset_nest_get_entity_form',
    'wrapper' => $wrapper,
  );
  return $form;
}

function asset_nest_entity_form_submit($form, &$form_state){
  // Submit asset form.
  asset_edit_form_submit($form, $form_state);

  // dsm($form_state);
  extract($form_state['asset_nest']);
  // $child_entity_type = $form_state['asset_nest']['entity_type'];
  $child_entity = $form_state[$child_entity_type];
  list($child_entity_id, , $child_entity_bundle) = entity_extract_ids($child_entity_type, $child_entity);

  $wrapper = entity_metadata_wrapper($parent_entity_type, $parent_entity);
  $current = $wrapper->{$parent_field_name}->value();
  $found = FALSE;
  foreach($current as $delta => $current_entity){
    list($current_entity_id, , ) = entity_extract_ids($child_entity_type, $current_entity);
    if($current_entity_id === $child_entity_id) $found = TRUE;
  }
  if(!$found){
    $wrapper->{$parent_field_name}[count($current)]->set($child_entity_id);
    $wrapper->save();
  }

  $form_state['rebuild'] = TRUE;
  $form_state['asset_nest_op'] = 'parent';
}

// function asset_nest_entity_form_ajax_submit($form, $form_state){
//   return $form;
//   $child_entity_type = $form_state['asset_nest']['entity_type'];
//   $child_entity = $form_state[$child_entity_type];
//   list($child_entity_id, , $bundle) = entity_extract_ids($child_entity_type, $child_entity);
//   $parent_entity_type = $form_state['asset_nest']['parent_entity_type'];
//   $parent_entity = $form_state['asset_nest']['parent_entity'];
//   $parent_field_name = $form_state['asset_nest']['parent_field_name'];

//   // Save parent entity with newly created child.
//   $wrapper = entity_metadata_wrapper($parent_entity_type, $parent_entity);
//   $current = $wrapper->{$parent_field_name}->value();
//   $found = FALSE;
//   foreach($current as $delta => $current_entity){
//     list($current_entity_id, , ) = entity_extract_ids($child_entity_type, $current_entity);
//     if($current_entity_id === $child_entity_id) $found = TRUE;
//   }
//   if(!$found){
//     $wrapper->{$parent_field_name}[count($current)]->set($child_entity_id);
//     $wrapper->save();
//   }

//   $child_entity_form = drupal_get_form($form_state['asset_nest']['parent_form_id'], $parent_entity);
//   return $child_entity_form;
//   $rendered_form = theme('status_messages') . render($child_entity_form);

//   $commands = array();
//   $commands[] = ajax_command_replace('#' . $form_state['asset_nest']['wrapper'], $rendered_form);
//   return array('#type' => 'ajax', '#commands' => $commands);
// }
