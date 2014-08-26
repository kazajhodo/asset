<?php

/**
 * Implements hook_form_FORM_ID_alter().
 */
function asset_form_field_ui_display_overview_form_alter(&$form, &$form_state){
  if($form['#entity_type'] == 'asset'){

    $form['additional_settings']['asset_modes'] = array(
      '#type' => 'fieldset',
      '#title' => t('Asset View Modes'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
      '#description' => t('View modes are shared across all entity types.')
    );
    $bundle = $form['#bundle'];
    $asset_type = $form['#asset_type'] = asset_type_load($bundle);

    $form['additional_settings']['asset_modes']['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Create new view mode'),
      '#maxlength' => 255,
    );
    $form['additional_settings']['asset_modes']['machine_name'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#required' => FALSE,
      '#machine_name' => array(
        'exists' => 'asset_form_field_ui_display_overview_form_alter_exists',
        'source' => array('additional_settings', 'asset_modes', 'name'),
      )
    );

    $view_modes = variable_get('asset_view_modes');
    if($view_modes && is_array($view_modes)){
      $form['additional_settings']['asset_modes']['remove'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Remove view modes'),
        '#options' => $view_modes,
      );
    }

    $form['#submit'][] = 'asset_form_field_ui_display_overview_form_alter_submit';
  }
}

function asset_form_field_ui_display_overview_form_alter_exists($value) {
  $entity_info = entity_get_info('asset');
  return isset($entity_info['view modes'][$value]);
}

/**
 * Save custom view modes and enable on current asset type.
 */
function asset_form_field_ui_display_overview_form_alter_submit($form, &$form_state){
  if(!empty($form_state['values']['asset_modes'])){
    $values = $form_state['values']['asset_modes'];
    $asset_type = $form['#asset_type'];
    $view_modes = variable_get('asset_view_modes', array());
    $changed = FALSE;
    if(!empty($values['name']) && !empty($values['machine_name'])){
      $changed = TRUE;
      drupal_set_message(t('View mode %view_mode has been created.', array('%view_mode' => $values['name'])));
      $view_modes[$values['machine_name']] = $values['name'];
      // Enable this view mode on this asset type.
      if($settings = variable_get('field_bundle_settings_asset__' . $asset_type->type)){
        $settings['view_modes'][$values['machine_name']] = array('custom_settings' => TRUE);
        variable_set('field_bundle_settings_asset__' . $asset_type->type, $settings);
      }
    }
    if(!empty($values['remove']) && is_array($values['remove'])){
      foreach($values['remove'] as $view_mode => $remove){
        if($remove){
          $changed = TRUE;
          drupal_set_message(t('View mode %view_mode has been removed.', array('%view_mode' => $view_modes[$view_mode])));
          unset($view_modes[$view_mode]);
          // Delete all settings for this view mode.
          foreach (asset_get_types() as $type => $info) {
            if($settings = variable_get('field_bundle_settings_asset__' . $type)){
              unset($settings['view_modes'][$view_mode]);
              variable_set('field_bundle_settings_asset__' . $type, $settings);
            }
          }
        }
      }
    }
    if($changed){
      variable_set('asset_view_modes', $view_modes);
      entity_info_cache_clear();
      menu_rebuild();
    }
  }
}
