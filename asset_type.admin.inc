<?php

/**
 * @file
 * Asset type editing UI.
 */

/**
 * UI controller.
 */
class AssetTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
		$items[$this->path]['description'] = 'Manage asset entity types, including adding
		and removing fields and the display of fields.';
    return $items;
  }
}

/**
 * Generates the asset type editing form.
 */
function asset_type_form($form, &$form_state, $asset_type, $op = 'edit') {

  drupal_add_css(drupal_get_path('module','asset') . '/css/asset.css');

  if ($op == 'clone') {
    $asset_type->label .= ' (cloned)';
    $asset_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $asset_type->label,
    '#description' => t('The human-readable name of this asset type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($asset_type->type) ? $asset_type->type : '',
    '#maxlength' => 32,
//    '#disabled' => $asset_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'asset_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this asset type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['data']['#tree'] = TRUE;

  $form['data']['plural'] = array(
    '#title' => t('Plural'),
    '#type' => 'textfield',
    '#default_value' => empty($asset_type->data['plural']) ? '' : $asset_type->data['plural'],
    '#description' => t('The human-readable plural name of this asset type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $options = array();
  foreach(fawesome_list() as $icon){
    $options[$icon] = '<i class="fa fa-'.$icon.'"></i>';
  }
  $form['data']['icon'] = array(
    '#type' => 'radios',
    '#title' => t('Toolbar Icon'),
    '#options' => $options,
    '#required' => TRUE,
    '#default_value' => empty($asset_type->data['icon']) ? array() : $asset_type->data['icon'],
    '#attributes' => array('class'=>array('clearfix')),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save asset type'),
    '#weight' => 40,
  );

  //Locking not supported yet
  /*if (!$asset_type->isLocked() && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete asset type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('asset_type_form_submit_delete')
    );
  }*/
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function asset_type_form_submit(&$form, &$form_state) {
  $asset_type = entity_ui_form_submit_build_entity($form, $form_state);
  $asset_type->save();
  $form_state['redirect'] = 'admin/structure/asset_types';
}

/**
 * Form API submit callback for the delete button.
 */
function asset_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/asset_types/manage/' . $form_state['asset_type']->type . '/delete';
}
