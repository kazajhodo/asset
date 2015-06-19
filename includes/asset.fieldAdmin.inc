<?php

/**
 * Implements hook_form_FIELD_UI_FIELD_EDIT_FORM_alter().
 */
function asset_form_field_ui_field_edit_form_alter(&$form, &$form_state) {

  // Get allowed field and widget types.
  $allowed_field_types = asset_allowed_field_types();
  $allowed_field = in_array($form['#field']['type'], $allowed_field_types);
  $allowed_widget_types = asset_allowed_widget_types();
  $allowed_widget = in_array($form['instance']['widget']['type']['#value'], $allowed_widget_types);

  // Add the asset settings to the field instance form.
  if ($allowed_field  && $allowed_widget) {
    asset_form_field_form($form, $form_state);
  }
}

/**
 * Asset configuration form.
 */
function asset_form_field_form(&$form, &$form_state){
  $instance = empty($form['#instance']) ? array() : $form['#instance'];

  // Get defaults
  $defaults = !empty($instance['settings']['asset']['settings']) ? $instance['settings']['asset']['settings'] : array();

  // Fieldset for asset settings on this field instance.
  $form['instance']['settings']['asset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assets'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#id' => 'edit-instance-settings-asset',
  );

  // Enable asset on this field instance.
  $form['instance']['settings']['asset']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Asset Support'),
    '#description' => t('Asset support enables asset replacement via tokens. [asset-1] will be replaced with a rendered version of Asset #1. [asset-1-2] will be replaced with a rendered version of Asset #1 and will use the instance settings of Asset Instance #2.'),
    '#default_value' => isset($instance['settings']['asset']['status']) ? $instance['settings']['asset']['status'] : 0,
  );

  $form['instance']['settings']['asset']['default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Default Asset Settings'),
    '#description' => t('Defaults are set via !url.', array('!url' => l('admin/structure/assets/settings','admin/structure/assets/settings'))),
    '#default_value' => isset($instance['settings']['asset']['default']) ? $instance['settings']['asset']['default'] : 1,
    '#element_validate' => array('asset_configuration_form_asset_default_validate'),
    '#states' => array(
      'visible' => array(
        'input[name="instance[settings][asset][status]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['instance']['settings']['asset']['settings'] = asset_configuration_form($defaults);

  $form['instance']['settings']['asset']['settings']['#states'] = array(
      'visible' => array(
        'input[name="instance[settings][asset][status]"]' => array('checked' => TRUE),
        'input[name="instance[settings][asset][default]"]' => array('checked' => FALSE),
      ),
  );
}

function asset_configuration_form_asset_default_validate($form, &$form_state){
  $values = &$form_state['values'];
  if(!empty($values['instance']['settings']['asset']['default'])){
    unset($values['instance']['settings']['asset']['settings']);
  }
}

/**
 * Settings form for Asset.
 */
function asset_configuration_form($defaults = array()){

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['info']['#markup'] = '<div class="description">' . t('Temporary placeholder for future settings.') . '</div>';

  drupal_alter('asset_configuration_form', $form, $default_values);
  return $form;
}
