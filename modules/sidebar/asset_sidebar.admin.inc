<?php

/**
 * Menu callback for settings form.
 */
function asset_sidebar_settings($form, $form_state) {

  $regions = system_region_list(variable_get('theme_default'));
  $form['asset_sidebar_region'] = array(
    '#type' => 'select',
    '#title' => t('Region to place sidebar'),
    '#options' => $regions,
    '#required' => TRUE,
    '#default_value' => variable_get('asset_sidebar_region'),
  );

  $form['asset_sidebar_padding'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sidebar padding'),
    '#description' => t('The margin around the sidebar in px or %. Example: 20px or 5%.'),
    '#tree' => TRUE
  );

  $defaults = variable_get('asset_sidebar_padding', array());

  $form['asset_sidebar_padding']['top'] = array(
    '#type' => 'textfield',
    '#title' => t('Top'),
    '#size' => 20,
    '#default_value' => !empty($defaults['top']) ? $defaults['top'] : '',
  );

  $form['asset_sidebar_padding']['right'] = array(
    '#type' => 'textfield',
    '#title' => t('Right'),
    '#size' => 20,
    '#default_value' => !empty($defaults['right']) ? $defaults['right'] : '',
  );

  $form['asset_sidebar_padding']['bottom'] = array(
    '#type' => 'textfield',
    '#title' => t('Bottom'),
    '#size' => 20,
    '#default_value' => !empty($defaults['bottom']) ? $defaults['bottom'] : '',
  );

  $form['asset_sidebar_padding']['left'] = array(
    '#type' => 'textfield',
    '#title' => t('Left'),
    '#size' => 20,
    '#default_value' => !empty($defaults['left']) ? $defaults['left'] : '',
  );

  return system_settings_form($form);
}
