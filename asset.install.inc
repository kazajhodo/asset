<?php

/**
 * Create a bundle
 */
function asset_install_bundle($type, $label, $plural, $data = array()){
  $bundles = field_info_bundles('asset');
  if (empty($bundles[$type])) {
    $controller = new AssetTypeController('asset_type');
    $params = array();
    $params['type'] = $type;
    $params['label'] = t($label);
    $params['data'] = $data;
    $params['data']['plural'] = $plural;
    $bundle = $controller->create($params);
    $controller->save($bundle);
    return TRUE;
  }
  return FALSE;
}


/**
 * A little helper that will output exportable information for the asset type,
 * fields, and field instances.
 */
function asset_install_helper($asset_type){
  $asset_type = asset_type_load($asset_type);
  dpm($asset_type);

  $fields = field_info_instances("asset", $asset_type->type);
  foreach($fields as $data){
    $field = field_info_field($data['field_name']);
    print '<h1>'.$data['label'].' | '.$data['field_name'].'</h1>';
    print '<h3>Field</h3>';
    print '<textarea style="background:#e9e9e9;" cols="50" rows="10">';
    var_export($field);
    print '</textarea>';
    $instance = field_info_instance("asset", $data['field_name'], $asset_type->type);
    print '<h3>Instance</h3>';
    print '<textarea style="background:#e9e9e9;" cols="50" rows="10">';
    var_export($instance);
    print '</textarea><hr>';
  }
}

/**
 * Install a field and instance.
 */
function asset_install_field($field_name = '', $bundle_name = '', $entity_type = 'asset'){
  $created = &drupal_static(__FUNCTION__);

  $bundles = field_info_bundles($entity_type);
  if (!empty($bundles[$bundle_name])) {

    // Install field
    $function = 'asset_install_' . $field_name;
    if(function_exists($function) && !empty($field_name)){
      if(empty($created['fields'][$field_name])){
        if(!field_info_field($field_name)){
          $field = $function($field_name);
          if(!empty($field)){
            field_create_field($field);
            $created['fields'][$field_name] = $field_name;
          }
        }
      }
    }
    else{
      drupal_set_message(t('Function %function does not exist. Field cannot be created.', array('%function'=>$function)) , 'warning');
    }

    // Install field instance
    $function = 'asset_install_' . $field_name . '_instance';
    if(function_exists($function) && !empty($field_name) && !empty($entity_type) && !empty($bundle_name)){
      if(empty($created['instances'][$field_name .'-' .$bundle_name])){
        if(field_info_field($field_name) && !field_info_instance($entity_type, $field_name, $bundle_name)){
          $instance = $function($field_name, $entity_type, $bundle_name);
          if(!empty($instance)){
            field_create_instance($instance);
            $created['instances'][$field_name .'-' .$bundle_name] = $field_name .'-' .$bundle_name;
          }
        }
      }
    }
    else{
      drupal_set_message(t('Function %function does not exist. Field cannot be created.', array('%function'=>$function)) , 'warning');
    }

  }
  else{
    drupal_set_message(t('Could not find bundle %bundle_name of asset type %entity_type', array('%bundle_name'=>$bundle_name, '%entity_type'=>$entity_type)),'warning');
  }
}
