<?php

/**
 * Create a bundle
 */
function asset_install_bundle($type, $label, $data = array()){
  $bundles = field_info_bundles('asset');
  if (empty($bundles[$type])) {
    $controller = new AssetTypeController('asset_type');
    $params = array();
    $params['type'] = $type;
    $params['label'] = t($label);
    $bundle = $controller->create($params);
    $controller->save($bundle);
    return TRUE;
  }
  return FALSE;
}
