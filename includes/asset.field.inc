<?php

/**
 * Implements hook_field_attach_presave().
 */
function asset_field_attach_presave($entity_type, $entity) {
  $wrapper = entity_metadata_wrapper($entity_type, $entity);
  list($entity_id, , $bundle_name) = entity_extract_ids($entity_type, $entity);
  $field_list = field_info_instances($entity_type, $bundle_name);
  $usage_calc = FALSE;
  foreach($field_list as $field_name => $instance){
    $field = field_info_field($field_name);
    if(isset($field['type']) && $field['type'] == 'entityreference' && $field['settings']['target_type'] == 'asset'){
      // If field being saved is an entity reference and references assets,
      // recalculate usage.
      $usage_calc = TRUE;
    }
    if(!empty($instance['settings']['asset']['status'])){
      $value = $wrapper->{$field_name}->value();
      // Remove usage for this field.
      asset_usage_purge(array('entity_type' => $entity_type, 'entity_id' => $entity_id, 'field_name' => $field_name));
      if(!empty($value['value'])){
        $text = $value['value'];
        foreach(asset_token_find($text) as $token => $info){
          // Save usage for this field.
          $usage = array(
            'entity_type' => $entity_type,
            'entity_id' => $entity_id,
            'bundle' => $bundle_name,
            'field_name' => $field_name,
            'asset_id' => $info['aid'],
            'instance_id' => $info['iid'],
          );
          asset_usage_save($usage);
        }

      }
    }
  }
  if($usage_calc){
    // Recalculate usage.
    asset_usage_clear();
  }
}

/**
 * Implements hook_preprocess_field().
 */
function asset_preprocess_field(&$vars) {
  if(in_array($vars['element']['#field_type'], asset_allowed_field_types())){
    $entity = $vars['element']['#object'];
    $entity_type = $vars['element']['#entity_type'];
    list($id,, $bundle) = entity_extract_ids($entity_type, $entity);
    $field = field_info_field($vars['element']['#field_name']);
    $instance = asset_field_info_instance($vars['element']['#entity_type'], $vars['element']['#field_name'], $vars['element']['#bundle']);

    // @TODO allowed types are now handled by eXo instead of the asset module.
    // This may need to be reconsidered.
    $allowed_types = array();
    // $allowed_types = !empty($instance['settings']['asset']['settings']['assets']) ? array_keys($instance['settings']['asset']['settings']['assets']) : array();
    // // Add asset js settings for other modules to use.
    // if(!empty($allowed_types) && !empty($instance['settings']['asset']['settings']['assets'])){
    //   $asset_types = asset_type_load_multiple($allowed_types);
    //   $settings = $instance['settings']['asset']['settings']['assets'];
    //   $js = array();
    //   foreach($settings as $asset_type => &$data){
    //     foreach($asset_types[$asset_type] as $key => $info){
    //       if(!empty($info)){
    //         $data[$key] = $info;
    //       }
    //     }
    //   }
    //   $js['asset']['fields'][$entity_type][$field['field_name']] = $settings;
    //   drupal_add_js($js, 'setting');
    // }
    if(!empty($instance['settings']['asset']['status'])){
      $preview = !empty($entity->asset_preview);
      foreach($vars['items'] as &$data){
        $data['#markup'] = asset_token_replace($data['#markup'], $allowed_types, 'asset', $preview);
      }
    }
  }
  if(is_a($vars['element']['#object'], 'Asset')){
    $vars['theme_hook_suggestions'][] = 'field__asset';
    $vars['theme_hook_suggestions'][] = 'field__asset__' . $vars['element']['#field_name'];
    $vars['element_type'] = isset($vars['element']['#element_type']) ? $vars['element']['#element_type'] : 'div';
  }
}

/**
 * Replace all tokens with their matching assets.
 *
 * @param $allowed_types
 *   An array of allowed asset types. If empty, all types will be allowed.
 */
function asset_token_replace($text, $allowed_types = array(), $view_mode = 'default', $preview = FALSE){

  // Regex to replace <p> tags that wrap assets.
  $text = preg_replace('/(<p>(\[.*\](\s)*)<\/p>)/i', '$2', $text);

  $assets = asset_token_find($text, TRUE);
  foreach($assets as $token => $info){
    $view = '';
    if(!empty($info['asset'])){
      if(!empty($allowed_types) && !in_array($info['asset']->type, $allowed_types)){
        $view = '';
      }
      else{
        $view = asset_render($info['asset'], $view_mode, $preview);
        $view = drupal_render($view);
      }
    }
    $text = str_replace($token, $view, $text);
  }
  return $text;
}

/**
 * Remove all tokens from text.
 */
function asset_token_remove($text){
  if (!empty($text)) {
    $pattern = '/(\[asset-[0-9]+(-[0-9]+)?\])/i';
    return preg_replace($pattern, '', $text);
  }
}

/**
 * Get all asset token matches
 *
 * @param  $assets_load
 *   If true, load the full asset object.
 */
function asset_token_find($text, $assets_load = FALSE){
  $assets = array();
  if (!empty($text)) {
    $pattern = '/(\[asset-[0-9]+(-[0-9]+)?\])/i';
    preg_match_all($pattern, $text, $matches);
    if(!empty($matches[0])){
      foreach($matches[0] as $match){
        $pattern = '/\[asset-([0-9]+)-?([0-9]+)?\]/i';
        preg_match($pattern, $match, $m);
        $asset_id = $key = $m[1];
        $instance_id = !empty($m[2]) ? $m[2] : asset_load_default_instance($asset_id);
        $asset = NULL;
        if($assets_load){
          $asset = asset_load($asset_id, $instance_id);
        }
        $assets[$match] = array(
          'aid' => $asset_id,
          'iid' => $instance_id,
          'asset' => $asset,
          // 'token' => $match
        );
      }
    }
  }
  return $assets;
}

/**
 * Return a list of allowed elements.
 * @TODO: Make this a hook.
 */
function asset_allowed_elements() {
  $allowed_elements = array(
    'textarea',
  );
  drupal_alter('asset_allowed_elements', $allowed_elements);
  return $allowed_elements;
}

/**
 * Return a list of allowed field types.
 * @TODO: Make this a hook.
 */
function asset_allowed_field_types() {
  $allowed_field_types = array(
    'text_long',
    'text_with_summary',
  );
  drupal_alter('asset_allowed_field_types', $allowed_field_types);
  return $allowed_field_types;
}

/**
 * Return a list of allowed widget types.
 * @TODO: Make this a hook.
 */
function asset_allowed_widget_types() {
  $allowed_widget_types = array(
    'text_textarea',
    'text_textarea_with_summary',
  );
  drupal_alter('asset_allowed_widget_types', $allowed_widget_types);
  return $allowed_widget_types;
}
