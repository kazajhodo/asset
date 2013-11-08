<?php

function asset_sidebar_form_wrapper($asset, $type = NULL, $id = NULL){
  module_load_include('inc','asset','asset.admin');
  $output = drupal_get_form('asset_sidebar_edit_form', $asset, $type, $id);
  return $output;

}

/**
 * Form callback: create or edit a asset.
 *
 * @param $asset
 *   The asset object to edit or for a create form an empty asset object
 *     with only a asset type defined.
 */
function asset_sidebar_edit_form($form, &$form_state, $asset, $type, $id) {
  $form += drupal_retrieve_form('asset_edit_form', $form_state, $asset);
  $form_state['build_info']['files'][] = drupal_get_path('module', 'asset').'/asset.admin.inc';
  $form_state['build_info']['files'][] = drupal_get_path('module', 'asset_sidebar').'/asset_sidebar.page.inc';

  $field = &$form['name'];
  if($id && $type) $field['#value'] = t('Sidebar for') . ' ' . $type . ' with id ' . $id;
  $field['#access'] = FALSE;

  // Hide sidebar id.
  $field = &$form['field_asset_sidebar_id'];
  if($id && $type) $field[$field['#language']][0]['value']['#value'] = $id;
  $field['#access'] = FALSE;
  // Hide sidebar type.
  $field = &$form['field_asset_sidebar_type'];
  if($id && $type) $field[$field['#language']][0]['value']['#value'] = $type;
  $field['#access'] = FALSE;

  return $form;
}
