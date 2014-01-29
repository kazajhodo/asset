<?php

/**
 * Inline entity form "add existing" alter
 */
function _asset_inline_entity_form_reference_form_alter(&$form, &$form_state){

  $ief_id = $form['#ief_id'];
  $instance = $form_state['inline_entity_form'][$ief_id]['instance'];
  $settings = $form_state['inline_entity_form'][$ief_id]['form settings'];
  module_load_include('inc', 'asset', 'asset.page');
  drupal_add_js(drupal_get_path('module','asset') . '/js/asset.inline_entity_form.js');

  $entity_type = $instance['entity_type'];
  $field_name = $instance['field_name'];
  $bundle = $instance['bundle'];

  $field = field_info_field($field_name);

  // Only act on assets.
  if($field['settings']['target_type'] !== 'asset') return;

  $controller = inline_entity_form_get_controller($instance);

  // @TODO allow for more than one bundle
  $asset_type = $settings['bundle'];
  $asset = asset_create(array('type' => $asset_type));

  $form['#prefix'] = '<div class="asset-inline-entity-browser">';
  $form['#suffix'] = '</div>';

  $form['browser'] = asset_browser($asset);

  // $form['entity_id']['#type'] = 'hidden';
  $form['entity_id']['#attributes'] = array('class'=>array('asset-inline-entity'));
  $form['entity_id']['#prefix'] = '<div class="element-hidden">';
  $form['entity_id']['#suffix'] = '</div>';

  $form['actions']['ief_reference_cancel']['#attributes']['class'][] = 'ief-entity-cancel';
}

/**
 * Inline entity form alter
 */
function _asset_inline_entity_form_entity_form_alter(&$form, &$form_state){

  if($form['#entity_type'] != 'asset') return;

  $entity = $form['#entity'];
  $asset_type = asset_type_load($entity->type);
  $form['#title'] = t('Add new @title', array('@title' => $asset_type->label));

  // Bind a submit callback to all ief actions
  foreach($form['actions'] as &$field){
    if(is_array($field) && $field['#type'] == 'submit'){
      $field['#submit'][] = 'asset_inline_entity_form_entity_form_submit';
    }
  }

  if(isset($form['actions']['ief_add_save'])){
    $form['actions']['ief_add_save']['#value'] = t('Create @title', array('@title' => $asset_type->label));
  }

  if(isset($form['actions']['ief_edit_save'])){
    $form['actions']['ief_edit_save']['#value'] = t('Update @title', array('@title' => $asset_type->label));
  }
}

/**
 * @file
 * Defines the base inline entity form controller.
 */

class AssetInlineEntityFormController extends EntityInlineEntityFormController {

  /**
   * Returns an array of css filepaths for the current entity type, keyed
   * by theme name.
   *
   * If provided, the "base" CSS file is included for all themes.
   * If a CSS file matching the current theme exists, it will also be included.
   *
   * @code
   * return array(
   *   'base' => drupal_get_path('module', 'test_module') . '/css/inline_entity_form.base.css',
   *   'seven' => drupal_get_path('module', 'test_module') . '/css/inline_entity_form.seven.css',
   * );
   * @endcode
   */
  public function css() {
    drupal_add_css(drupal_get_path('module','asset') . '/css/asset.css');
    return array();
  }

  /**
   * Returns an array of fields used to represent an entity in the IEF table.
   *
   * The fields can be either Field API fields or properties defined through
   * hook_entity_property_info().
   *
   * Modules can alter the output of this method through
   * hook_inline_entity_form_table_fields_alter().
   *
   * @param $bundles
   *   An array of allowed bundles for this widget.
   *
   * @return
   *   An array of field information, keyed by field name. Allowed keys:
   *   - type: 'field' or 'property',
   *   - label: Human readable name of the field, shown to the user.
   *   - weight: The position of the field relative to other fields.
   *   Special keys for type 'field', all optional:
   *   - formatter: The formatter used to display the field, or "hidden".
   *   - settings: An array passed to the formatter. If empty, defaults are used.
   *   - delta: If provided, limits the field to just the specified delta.
   */
  public function tableFields($bundles) {
    $info = entity_get_info($this->entityType);
    $metadata = entity_get_property_info($this->entityType);

    $fields = array();

    // If only one product type is allowed, its fields can be used as columns.
    if (count($bundles) == 1) {
      $bundle = reset($bundles);

      foreach (field_info_instances('asset', $bundle) as $field_name => $instance) {
        $field = field_info_field($field_name);

        // If the product has an imagefield, show it.
        if ($field['type'] == 'image') {
          // Determine the correct image style to use.
          $image_style = 'thumbnail';
          if (!empty($instance['widget']['settings']['preview_image_style'])) {
            $image_style = $instance['widget']['settings']['preview_image_style'];
          }

          $fields[$field_name] = array(
            'type' => 'field',
            'label' => $instance['label'],
            'formatter' => 'image',
            'settings' => array('image_style' => $image_style),
            'delta' => 0,
            'weight' => -10,
          );
          // Don't add any other imagefields. One is enough.
          break;
        }
      }
    }
    if (!empty($info['entity keys']['label'])) {
      $label_key = $info['entity keys']['label'];
      $fields[$label_key] = array(
        'type' => 'property',
        'label' => $metadata ? $metadata['properties'][$label_key]['label'] : t('Label'),
        'weight' => 1,
      );
    }
    else {
      $id_key = $info['entity keys']['id'];
      $fields[$id_key] = array(
        'type' => 'property',
        'label' => $metadata ? $metadata['properties'][$id_key]['label'] : t('ID'),
        'weight' => 1,
      );
    }
    if (count($bundles) > 1) {
      $bundle_key = $info['entity keys']['bundle'];
      $fields[$bundle_key] = array(
        'type' => 'property',
        'label' => $metadata ? $metadata['properties'][$bundle_key]['label'] : t('Type'),
        'weight' => 2,
      );
    }

    return $fields;
  }

  /**
   * Returns the entity form to be shown through the IEF widget.
   *
   * When adding data to $form_state it should be noted that there can be
   * several IEF widgets on one master form, each with several form rows,
   * leading to possible key collisions if the keys are not prefixed with
   * $entity_form['#parents'].
   *
   * @param $entity_form
   *   The entity form.
   * @param $form_state
   *   The form state of the parent form.
   */
  public function entityForm($entity_form, &$form_state) {
    $info = entity_get_info($this->entityType);
    $entity = $entity_form['#entity'];
    $asset_type = asset_type_load($entity->type);

    if (!empty($info['fieldable'])) {
      $langcode = entity_language($this->entityType, $entity);
      if(!empty($entity->id)){
        $entity_form['notice'] = array(
          '#markup' => '<div class="messages warning">Editing this '.strtolower($asset_type->label).' asset will impact all other areas where this asset is used.</div>',
          '#weight' => -6,
        );
      }
      $entity_form['name'] = array(
        '#type' => 'textfield',
        '#title' => t($asset_type->label . ' Name'),
        '#default_value' => isset($entity->name) ? $entity->name : '',
        '#maxlength' => 255,
        '#required' => TRUE,
        '#weight' => -5,
      );
      field_attach_form($this->entityType, $entity, $entity_form, $form_state, $langcode);
    }

    return $entity_form;
  }
}
