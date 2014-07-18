<?php

/**
 * @file
 * Defines the base inline entity form controller.
 */

class AssetInlineEntityFormController extends EntityInlineEntityFormController{

  /**
   * Returns the default entity type labels.
   */
  public function defaultLabels() {
    $labels = array(
      'singular' => t('asset'),
      'plural' => t('assets'),
    );
    return $labels;
  }

  /**
   * Returns an array of default settings in the form of key => value.
   */
  public function defaultSettings() {
    $defaults = parent::defaultSettings();
    $defaults['instance_default_creation'] = FALSE;
    return $defaults;
  }

  /**
   * Returns the settings form for the current entity type.
   *
   * The settings form is embedded into the IEF widget settings form.
   * Settings are later injected into the controller through $this->settings.
   *
   * @param $field
   *   The definition of the reference field used by IEF.
   * @param $instance
   *   The definition of the reference field instance.
   */
  public function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);
    $form['instance_default_creation'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Allow default instance to be created/edited'),
      '#default_value' => $this->settings['instance_default_creation'],
    );
    return $form;
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

        if ($field['type'] == 'video_embed_field'){
          // Determine the correct image style to use.
          $image_style = 'thumbnail';
          if (!empty($instance['widget']['settings']['preview_image_style'])) {
            $image_style = $instance['widget']['settings']['preview_image_style'];
          }

          $fields[$field_name] = array(
            'type' => 'field',
            'label' => $instance['label'],
            'formatter' => 'video_embed_field_thumbnail',
            'settings' => array('image_style' => $image_style),
            'delta' => 0,
            'weight' => -10,
          );
          // Don't add any other imagefields. One is enough.
          break;
        }

        if ($field['type'] == 'file'){
          $fields[$field_name] = array(
            'type' => 'field',
            'label' => $instance['label'],
            'formatter' => 'file_default',
            // 'settings' => array('image_style' => $image_style),
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
    $instance = $entity->instance;

    if (!empty($info['fieldable'])) {
      $langcode = entity_language($this->entityType, $entity);
      if(!empty($entity->id) && empty($asset_type->data['no_multiple'])){
        $entity_form['notice'] = array(
          '#markup' => '<div class="alert-box warning">Editing this '.strtolower($asset_type->name).' asset will impact all other areas where this asset is used.</div>',
          '#weight' => -6,
        );
      }
      if(empty($asset_type->data['auto_title'])){
        $entity_form['name'] = array(
          '#type' => 'textfield',
          '#title' => t($asset_type->name . ' Name'),
          '#default_value' => isset($entity->name) ? $entity->name : '',
          '#maxlength' => 255,
          '#required' => TRUE,
          '#weight' => -5,
        );
      }
      field_attach_form($this->entityType, $entity, $entity_form, $form_state, $langcode);
    }

    if($this->settings['instance_default_creation'] || !empty($asset_type->data['require_instance'])){
      $entity_form['instance_form'] = array(
        '#access' => FALSE,
        '#tree' => TRUE,
        '#weight' => 10,
      );
      $entity_form['instance_form']['status'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('Set default options'),
        '#default_value' => !empty($entity->instance_id) ? 1 : 0,
      );
      $entity_form['instance_form']['data'] = array(
        '#type' => 'fieldset',
        '#tree' => TRUE,
        '#title' => t('Instance Settings')
      );

      if(!empty($asset_type->data['require_instance'])){
        // If instances are required for this asset type, disable the option
        // to turn them off.
        $entity_form['instance_form']['status'] = array(
          '#type' => 'value',
          '#value' => 1,
        );
        $entity_form['instance_form']['data']['#type'] = 'item';
        $entity_form['instance_form']['data']['#title'] = NULL;
      }

      if($instance_fields = $instance->widgetFields()){
        $entity_form['instance_form']['#access'] = TRUE;
        $parents = asset_parents_to_name($entity_form['#parents']);
        $entity_form['instance_form']['data']['#states'] = array(
          'visible' => array(
            ':input[name="'.$parents.'[instance_form][status]"]' => array('checked' => TRUE),
          ),
        );
        $entity_form['instance_form']['data'] += $instance_fields;
      }
    }

    // Process multilingual and shared form elements.
    if(method_exists($this,'entityFormSharedElements')) $this->entityFormSharedElements($entity_form);

    return $entity_form;
  }

}

/**
 * Inline entity form "add existing" alter
 */
function _asset_inline_entity_form_reference_form_alter(&$form, &$form_state){
  $ief_id = $form['#ief_id'];
  $instance = $form_state['inline_entity_form'][$ief_id]['instance'];
  $settings = $form_state['inline_entity_form'][$ief_id]['form settings'];

  $entity_type = $instance['entity_type'];
  $field_name = $instance['field_name'];
  $bundle = $instance['bundle'];
  $field = field_info_field($field_name);

  // Only act on assets.
  if($field['settings']['target_type'] !== 'asset') return;

  $form['#prefix'] = '<div class="asset-inline-entity-browser">';
  $form['#suffix'] = '</div>';

  // @TODO allow for more than one bundle
  $asset_type_name = $settings['bundle'];
  $asset_type = asset_type_load($asset_type_name);

  $form['browser'] = asset_type_browser($asset_type);

  $form['entity_id']['#attributes'] = array('class'=>array('asset-inline-entity'));
  $form['entity_id']['#prefix'] = '<div class="element-hidden">';
  $form['entity_id']['#suffix'] = '</div>';

  $form['actions']['ief_reference_save']['#attributes']['class'][] = 'primary';
  $form['actions']['ief_reference_save']['#attributes']['class'][] = 'small';
  $form['actions']['ief_reference_cancel']['#attributes']['class'][] = 'text';
  $form['actions']['ief_reference_cancel']['#attributes']['class'][] = 'small';
}

/**
 * Inline entity form alter
 */
function _asset_inline_entity_form_entity_form_alter(&$form, &$form_state){
  return;

  if($form['#entity_type'] != 'asset') return;

  $entity = $form['#entity'];
  $asset_type = asset_type_load($entity->type);
  $form['#title'] = t('Add New @title', array('@title' => $asset_type->name));

  $form['actions']['#type'] = 'actions';

  // Bind a submit callback to all ief actions
  // foreach($form['actions'] as &$field){
  //   if(is_array($field) && $field['#type'] == 'submit'){
  //     $field['#submit'][] = 'asset_inline_entity_form_entity_form_submit';
  //   }
  // }

  if(isset($form['actions']['ief_add_save'])){
    $form['actions']['ief_add_save']['#value'] = t('Create @title', array('@title' => $asset_type->name));
    $form['actions']['ief_add_save']['#attributes']['class'][] = 'primary';
    $form['actions']['ief_add_save']['#attributes']['class'][] = 'small';
  }

  if(isset($form['actions']['ief_add_cancel'])){
    $form['actions']['ief_add_cancel']['#attributes']['class'][] = 'text';
    $form['actions']['ief_add_cancel']['#attributes']['class'][] = 'small';
  }

  if(isset($form['actions']['ief_edit_save'])){
    $form['actions']['ief_edit_save']['#value'] = t('Update @title', array('@title' => $asset_type->name));
    $form['actions']['ief_edit_save']['#attributes']['class'][] = 'primary';
    $form['actions']['ief_edit_save']['#attributes']['class'][] = 'small';
  }

  if(isset($form['actions']['ief_edit_cancel'])){
    $form['actions']['ief_edit_cancel']['#attributes']['class'][] = 'text';
    $form['actions']['ief_edit_cancel']['#attributes']['class'][] = 'small';
  }
}

/**
 * Inline entity form widget alter.
 */
function _asset_field_widget_inline_entity_form_form_alter(&$element, &$form_state, $context){
  return;
  $field = $context['field'];

  // Only act on assets.
  if($field['settings']['target_type'] !== 'asset') return;

  // $instance = $context['instance'];
  // $widget = $instance['widget'];
  // $settings = inline_entity_form_settings($field, $instance);
  // $entity_info = entity_get_info($settings['entity_type']);
  // $cardinality = $field['cardinality'];
  // $controller = inline_entity_form_get_controller($instance);

  // The current entity type is not supported, execution can't continue.
  // if (!$controller) {
  //   return array();
  // }

  // dsm($element);
  if(isset($element['actions'])){
    $element['actions']['#attributes'] = array();
    $element['actions']['#type'] = 'actions';
    if(isset($element['actions']['ief_add'])){
      $element['actions']['ief_add']['#attributes']['class'][] = 'secondary';
      $element['actions']['ief_add']['#attributes']['class'][] = 'tiny';
    }
    if(isset($element['actions']['ief_add_existing'])){
      $element['actions']['ief_add_existing']['#attributes']['class'][] = 'secondary';
      $element['actions']['ief_add_existing']['#attributes']['class'][] = 'tiny';
    }
  }
}
