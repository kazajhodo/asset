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

  return system_settings_form($form);
}
