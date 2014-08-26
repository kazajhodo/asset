<?php

/**
 * Form callback wrapper: create or edit a asset.
 *
 * @param $asset
 *   The asset object being edited by this form.
 *
 * @see asset_edit_form()
 */
function asset_instance_form_wrapper($asset, $instance) {
  if(!is_a($instance, 'AssetInstance')){
    drupal_set_title(asset_instance_create_form_title($asset));
    $instance = asset_instance_create($asset);
  }
  // Add the breadcrumb for the form's location.
  asset_instance_set_breadcrumb($asset, $instance);
  return drupal_get_form('asset_instance_edit_form', $asset, $instance);
}

/**
 * Form callback: create or edit a asset.
 *
 * @param $asset
 *   The asset object to edit or for a create form an empty asset object
 *     with only a asset type defined.
 */
function asset_instance_edit_form($form, &$form_state, $asset, $instance) {
  $form_state['entity_type'] = 'asset_instance';
  $form_state['asset'] = $asset;
  $form_state['asset_instance'] = $instance;
  $form_state['wrapper_callback'] = 'entity_ui_main_form_defaults';

  $form['data'] = array();
  $form['data']['#tree'] = TRUE;

  if($instance_fields = $instance->widgetFields()){
    $form['data'] += $instance_fields;
  }

  $form['actions'] = array(
    '#type'       => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight'     => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type'   => 'submit',
    '#value'  => t('Save Asset Instance'),
    '#submit' => $submit + array('asset_instance_edit_form_submit'),
  );

  if (!empty($instance->id)) {
    $title = 'Delete';
    if(function_exists('fett_icon')) fett_icon($title);
    $form['actions']['delete']['#markup'] = l($title, 'admin/content/assets/manage/'.$asset->id.'/instance/'.$instance->id.'/delete', array('html'=>TRUE, 'attributes'=>array('class'=>array('button','tiny','text'))));
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'asset_instance_edit_form_validate';
  $form['#submit'][] = 'asset_instance_edit_form_submit';
  return $form;
}

/**
 * Form API validate callback for the asset form
 */
function asset_instance_edit_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $instance = $form_state['asset_instance'];
  $instance->widgetFieldsValidate($values['data']);
}

/**
 * Form API submit callback for the type form.
 */
function asset_instance_edit_form_submit($form, &$form_state) {
  $values = &$form_state['values'];
  $asset = $form_state['asset'];
  $instance = $form_state['asset_instance'];
  $instance->widgetFieldsSubmit($values['data']);
  $instance->data = $values['data'];
  $instance->save();
  $form_state['asset_instance'] = $instance;
  $form_state['redirect'] = array('asset/' . $asset->id, array('fragment' => $instance->id));
}

/**
 * Form callback: confirmation form for deleting a asset.
 *
 * @param $asset
 *   The asset to delete
 *
 * @see confirm_form()
 */
function asset_instance_delete_form($form, &$form_state, $asset, $instance) {
  $form_state['asset'] = $asset;
  $form_state['asset_instance'] = $instance;
  $usage = asset_usage($asset->id);

  $form['#submit'][] = 'asset_instance_delete_form_submit';

  $message = '<p>'.t('This action cannot be undone.').'</p>';
  if(isset($usage['content'][$instance->id])){
    $form['#attached']['css'][] = drupal_get_path('module', 'asset') . '/css/asset.full.scss';

    $message = '<div class="alert-box alert"><h4>'.t('This action cannot be undone.').'</h4><strong>'.t('This asset is currently being used.').'</strong> '.t('Deleting this asset instance will reset all areas where the asset is used to their default display settings.').'</div>';

    $form['usage'] = array(
      '#theme' => 'asset_usage_instance',
      '#usage' => $usage['content'][$instance->id],
      '#weight' => 1,
    );
  }

  $form = confirm_form($form,
    t('Are you sure you want to delete instance %instance of asset %name?', array('%instance' =>'#'.$instance->id, '%name' => $asset->name)),
    'asset/' . $asset->id,
    $message,
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for asset_delete_form
 */
function asset_instance_delete_form_submit($form, &$form_state) {
  $asset = $form_state['asset'];
  $instance = $form_state['asset_instance'];
  $instance->delete();
  drupal_set_message(t('Instance %instance of asset %name has been deleted.', array('%instance' =>'#'.$instance->id, '%name' => $asset->name)));
  watchdog('asset', 'Deleted asset instance %instance.', array('%instance' => $instance->id));

  $form_state['redirect'] = 'asset/' . $asset->id;
}

/**
 * Title for management pages.
 */
function asset_instance_create_form_title(Asset $asset){
  return 'Create new instance for ' . $asset->name;
}

/**
 * Title for management pages.
 */
function asset_instance_form_title(Asset $asset, AssetInstance $instance, $op = 'view'){
  return ucfirst($op) . ' ' . $asset->name . ' instance #' . $instance->id;
}

/**
 * Sets the breadcrumb for administrative asset pages.
 */
function asset_instance_set_breadcrumb($asset, $instance) {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('Assets'), 'admin/content/assets'),
  );

  drupal_set_breadcrumb($breadcrumb);
}
