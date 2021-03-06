<?php

/**
 * @file
 * Defines the base inline entity form controller.
 */

class AssetInlineEntityFormController extends EntityInlineEntityFormController{

  public function __construct($entityType, array $settings) {
    $this->entityType = $entityType;
    $this->settings = drupal_array_merge_deep($this->defaultSettings(), $settings);
  }

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
    $defaults['auto_title'] = false;
    $defaults['auto_title_pattern'] = '';
    $defaults['instance_default_creation'] = false;
    $defaults['allow_table_media'] = true;
    $defaults['allow_table_status'] = false;
    if(module_exists('manualcrop')){
      $field = field_info_field(arg(6));
      if(!$field){
        // Account fields have different path.
        $field = field_info_field(arg(5));
      }
      if($fields = asset_entityreference_contains($field, 'image')){
        $defaults['manualcrop'] = array();
        foreach($fields as $key => $field){
          $defaults['manualcrop'][$key]['manualcrop_enable'] = 0;
          $defaults['manualcrop'][$key]['manualcrop'] = manualcrop_default_widget_settings();
        }
      }
    }
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
    $form['auto_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide title field and automatically generate title'),
      '#default_value' => $this->settings['auto_title'],
    );

    $form['auto_title_pattern'] = array(
      '#type' => 'textfield',
      '#title' => t('Pattern for the title'),
      '#description' => t('Leave blank to use the default generated title. The parent title is available by using [parent:title].'),
      '#default_value' => $this->settings['auto_title_pattern'],
      '#states' => array(
        'visible' => array(
          ':input[name="instance[widget][settings][type_settings][auto_title]"]' => array('checked' => true),
        ),
      ),
      '#field_suffix' => module_exists('token') ? theme('token_tree', array('token_types' => array('asset','user'), 'dialog' => true)) : '',
    );
    // $form['instance_default_creation'] = array(
    //   '#type' => 'checkbox',
    //   '#title' => t('Allow default instance to be created/edited'),
    //   '#default_value' => $this->settings['instance_default_creation'],
    // );
    $form['allow_table_media'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show media thumbnails in the editor table UI'),
      '#default_value' => $this->settings['allow_table_media'],
    );
    $form['allow_table_status'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show published status in the editor table UI'),
      '#default_value' => $this->settings['allow_table_status'],
    );

    if(count($field['settings']['handler_settings']['target_bundles']) == 1){
      $asset_type = asset_type_load(reset($field['settings']['handler_settings']['target_bundles']));
      $form['override_labels']['#default_value'] = true;
      $form['label_singular']['#default_value'] = !empty($this->settings['label_singular']) && $this->settings['label_singular'] != 'asset' ? $this->settings['label_singular'] : strtolower($asset_type->title);
      $form['label_plural']['#default_value'] = !empty($this->settings['label_plural']) && $this->settings['label_plural'] != 'assets' ? $this->settings['label_plural'] : strtolower($asset_type->plural);
    }

    if(module_exists('manualcrop') && $targets = asset_entityreference_contains($field, 'image')){
      module_load_include('inc', 'manualcrop', 'manualcrop.admin');

      $form['manualcrop'] = array(
        '#type' => 'fieldset',
        '#title' => t('Manual Crop'),
        '#collapsible' => true,
        '#collapsed' => true,
        '#tree' => true,
        '#element_validate' => array('asset_inline_entity_manualcrop_validate'),
      );

      foreach($targets as $key => $target){
        $label = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
        $manualcrop = manualcrop_field_widget_settings_form('image_image', $this->settings['manualcrop'][$key]['manualcrop']);
        unset($manualcrop['manualcrop']['#parents']);

        if($this->settings['manualcrop'][$key]['manualcrop_enable']){
          $form['manualcrop']['#collapsed'] = false;
        }

        $form['manualcrop'][$key]['manualcrop_enable'] = $manualcrop['manualcrop']['manualcrop_enable'];
        $form['manualcrop'][$key]['manualcrop_enable']['#title'] = t('Enable cropping for %field_name', array('%field_name' => $label));
        unset($manualcrop['manualcrop']['manualcrop_enable']);

        $manualcrop['manualcrop']['#title'] = t('Crop settings for %field_name', array('%field_name' => $label));
        $manualcrop['manualcrop']['manualcrop_enable'] = array(
          '#type' => 'value',
          '#value' => 1,
        );
        $manualcrop['manualcrop'] += array(
          '#states' => array(
            'visible' => array(
              ':input[name="instance[widget][settings][type_settings][manualcrop]['.$key.'][manualcrop_enable]"]' => array('checked' => true),
            ),
          ),
        );

        $form['manualcrop'][$key] += $manualcrop;
      }
    }

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

    if(!empty($this->settings['allow_table_media'])) {
      // If only one product type is allowed, its fields can be used as columns.
      if (count($bundles) == 1) {
        $bundle = reset($bundles);
        $fields += $this->tableImage($bundle);
      } else {
        foreach($bundles as $bundle){
          $fields += $this->tableImage($bundle);
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

    if(!empty($this->settings['allow_table_status'])) {
      // $label_key = $info['entity keys']['status'];
      $fields['status'] = array(
        'type' => 'property',
        'label' => t('Status'),
        'sanitized' => TRUE,
        'weight' => 1,
      );
    }

    return $fields;
  }

  /**
   * Get the table image field if found.
   */
  public function tableImage($bundle) {
    $fields = array();
    foreach (field_info_instances('asset', $bundle) as $field_name => $instance) {
      $field = field_info_field($field_name);

      // If the entity has an imagefield, show it.
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

      // If the entity has a entityreference that contains an image field.
      if ($field['type'] == 'entityreference'){
        if($targets = asset_entityreference_contains($field, 'image')){
          foreach($targets as $key => $target){
            // Determine the correct image style to use.
            $image_style = 'thumbnail';
            if (!empty($target_instance['widget']['settings']['preview_image_style'])) {
              $image_style = $target_instance['widget']['settings']['preview_image_style'];
            }
            $fields[$field_name] = array(
              'type' => 'field',
              'label' => $instance['label'],
              'formatter' => 'asset_image',
              'settings' => array('field_name' => $key, 'image_style' => $image_style),
              'delta' => 0,
              'weight' => -10,
            );
            break;
          }
        }
      }
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
    $extra = field_info_extra_fields($this->entityType, $entity->type, 'form');

    if (!empty($info['fieldable'])) {
      $langcode = entity_language($this->entityType, $entity);
      if(!empty($entity->id) && empty($asset_type->data['no_multiple']) && empty($this->settings['delete_references']) && $usage = asset_usage($entity->id)){
        $asset_type = asset_type_load($entity->type);
        if(!empty($asset_type->data['usage_info'])){
          $entity_form['notice'] = array(
            '#markup' => '<div class="alert-box messages warning">Editing this '.strtolower($asset_type->title).' asset will impact all other areas where this asset is used.</div>',
            '#weight' => 6,
          );
          $entity_form['usage'] = array(
            '#theme' => 'asset_usage',
            '#usage' => $usage,
            '#weight' => 7,
          );
          $remove_form['usage']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/css/asset.full.scss';
        }
      }
      if(!$this->settings['auto_title'] && empty($asset_type->data['auto_title'])){
        $entity_form['title'] = array(
          '#type' => 'textfield',
          '#title' => !empty($asset_type->data['title_label']) ? t($asset_type->data['title_label']) : t($asset_type->title . ' Title'),
          '#default_value' => isset($entity->title) ? $entity->title : '',
          '#maxlength' => 255,
          '#required' => true,
          '#weight' => isset($extra['title']['weight']) ? $extra['title']['weight'] : -5,
        );
      }
      if(!empty($asset_type->data['allow_status'])){
        $entity_form['status'] = array(
          '#type'          => 'checkbox',
          '#title'         => t('Published'),
          '#default_value' => $entity->status,
          '#weight' => isset($extra['status']['weight']) ? $extra['status']['weight'] : 100,
        );
      }
      field_attach_form($this->entityType, $entity, $entity_form, $form_state, $langcode);
    }

    if($this->settings['instance_default_creation'] || !empty($asset_type->data['require_instance'])){
      $entity_form['instance_form'] = array(
        '#access' => false,
        '#tree' => true,
        '#weight' => 10,
      );
      $entity_form['instance_form']['status'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('Set default options'),
        '#default_value' => !empty($entity->instance_id) ? 1 : 0,
      );
      $entity_form['instance_form']['data'] = array(
        '#type' => 'fieldset',
        '#tree' => true,
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
        $entity_form['instance_form']['#access'] = true;
        $parents = asset_parents_to_name($entity_form['#parents']);
        $entity_form['instance_form']['data']['#states'] = array(
          'visible' => array(
            ':input[name="'.$parents.'[instance_form][status]"]' => array('checked' => true),
          ),
        );
        $entity_form['instance_form']['data'] += $instance_fields;
      }
    }

    // Process multilingual and shared form elements.
    if(method_exists($this,'entityFormSharedElements')) $this->entityFormSharedElements($entity_form);

    return $entity_form;
  }

  /**
   * Returns the remove form to be shown through the IEF widget.
   *
   * @param $remove_form
   *   The remove form.
   * @param $form_state
   *   The form state of the parent form.
   */
  public function removeForm($remove_form, &$form_state) {
    $remove_form = parent::removeForm($remove_form, $form_state);
    $remove_form['message']['#markup'] .= '<br>';
    $parents = asset_parents_to_name($remove_form['#parents']);
    $remove_form['usage'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="'.$parents.'[delete]"]' => array('checked' => true),
        ),
      ),
    );
    $remove_form['usage']['info']['#markup'] = 'test';
    $asset = $remove_form['#entity'];
    if($usage = asset_usage($asset->id)){
      $asset_type = asset_type_load($asset->type);
      if(!empty($asset_type->data['usage_info'])){
        $remove_form['usage']['warning']['#markup'] = '<div class="alert-box messages error"><strong>'.t('This %type is currently being used.', array('%type' => strtolower($asset_type->title))).'</strong> '.t('Deleting this asset remove it from anywhere it is used.').'</div>';
        $remove_form['usage']['info'] = array(
          '#theme' => 'asset_usage',
          '#usage' => $usage,
          '#weight' => 1,
        );
        $remove_form['usage']['info']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/css/asset.full.scss';
      }
    }
    return $remove_form;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormSubmit().
   */
  public function entityFormSubmit(&$entity_form, &$form_state) {

    parent::entityFormSubmit($entity_form, $form_state);


    // Set the title if needed.
    $pattern = ($this->settings['auto_title'] && $this->settings['auto_title_pattern']) ? $this->settings['auto_title_pattern'] : '';
    if($pattern){
      $parents = $entity_form['#parents'];
      if($entity_form['#op'] == 'add'){
        $parents = array_slice($parents, 0, -3);
      }
      else{
        $parents = array_slice($parents, 0, -5);
      }
      if(($parent_entity_values = drupal_array_get_nested_value($form_state['input'], $parents))){
        $title = !empty($parent_entity_values['title']) ? $parent_entity_values['title'] : '(System)';
        $pattern = str_replace('[parent:title]', $title, $pattern);
      }
    }
    entity_get_controller('asset')->setTitle($entity_form['#entity'], $pattern, $force = true);
  }

}

/**
 * Inline entity form "add existing" alter
 */
function _asset_inline_entity_form_reference_form_alter(&$form, &$form_state){
  $ief_id = $form['#ief_id'];
  $instance = $form_state['inline_entity_form'][$ief_id]['instance'];
  $settings = $form_state['inline_entity_form'][$ief_id]['settings'];

  $entity_type = $instance['entity_type'];
  $field_name = $instance['field_name'];
  $bundle = $instance['bundle'];
  $field = field_info_field($field_name);

  // Only act on assets.
  if($field['settings']['target_type'] !== 'asset') return;

  $bundle_count = count($settings['bundles']);
  $attributes = array();
  $attributes['class'][] = 'asset-inline-entity-browser';

  // Support for multiple bundles.
  $attributes['class'][] = count($settings['bundles']) > 1 ? 'multiple' : 'single';
  if($bundle_count > 1){
    $form['types'] = array(
      '#type' => 'vertical_tabs',
      '#weight' => -10,
    );
  }
  foreach($settings['bundles'] as $asset_type_name){
    $asset_type = asset_type_load($asset_type_name);
    if($bundle_count > 1){
      $form['types'][$asset_type_name] = array(
        '#type' => 'fieldset',
        '#title' => t('Existing %type', array('%type' => $asset_type->plural)),
      );
    }
    $form['types'][$asset_type_name]['browser'] = asset_type_browser($asset_type);
  }
  $form['#prefix'] = '<div'.drupal_attributes($attributes).'>';
  $form['#suffix'] = '</div>';

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
  if(isset($form['#groups'])){
    $ief_hide = array();
    $ief_show = array();
    foreach($form['#groups'] as $group){
      if(!empty($group->format_settings['instance_settings']['ief_hide'])){
        $ief_hide[] = $group->group_name;
      }
      else{
        $ief_show[] = $group->group_name;
      }
    }
    if(!empty($ief_hide)){
      field_group_hide_field_groups($form, $ief_hide);
      if(!empty($ief_show)){
        foreach($ief_show as $group_name){
          $group = &$form['#groups'][$group_name];
          if($group->format_type == 'tab'){
            $group->format_type = 'fieldset';
          }
        }
      }
    }
  }
}

/**
 * Inline entity form widget alter.
 */
function _asset_field_widget_inline_entity_form_form_alter(&$element, &$form_state, $context){
  $field = $context['field'];
  $instance = $context['instance'];

  // Only act on assets.
  if($field['settings']['target_type'] !== 'asset') return;

  $entity_type = $field['settings']['target_type'];
  $settings = $instance['widget']['settings']['type_settings'];
  if(module_exists('manualcrop') && !empty($settings['manualcrop']) && $targets = asset_entityreference_contains($field, 'image')){
    foreach($targets as $key => $target){
      // Check to make sure this target is enabled for cropping
      if(!isset($settings['manualcrop'][$key])){
        continue;
      }
      // We remove the first parent element since we will be providing the child entity.
      array_shift($target['parents']);
      foreach (element_children($element['entities']) as $i) {
        $row = &$element['entities'][$i];
        if(isset($row['actions']['ief_entity_edit'])){
          $edit = $row['actions']['ief_entity_edit'];
          $wrappers = asset_entityreference_get($entity_type, $row['#entity'], $target);
          foreach($wrappers as $wrapper){
            $file = $wrapper->{$target['field_name']}->value();
            $row['actions']['ief_entity_crop_' . $key] = array();
            if(!empty($file['fid']) && empty($file['uri'])){
              $file = file_load($file['fid']);
            }
            $element['#build_id'] = 'asset_ief_manualcrop';
            manualcrop_croptool_process($element, $form_state, $row['actions']['ief_entity_crop_' . $key], (object) $file, $settings['manualcrop'][$key]['manualcrop']);
          }
        }
      }
    }
  //   foreach (element_children($element['entities']) as $key) {
  //     $entity = &$element['entities'][$key];
  //     if(isset($entity['actions']['ief_entity_edit'])){
  //       $edit = $entity['actions']['ief_entity_edit'];
  //       $wrapper = asset_entityreference_get($entity['#entity'], $target);
  //       $file = $wrapper->{$target['field_name']}->value();
  //       $entity['actions']['ief_entity_crop'] = array();
  //       if(!empty($file['fid']) && empty($file['uri'])){
  //         $file = file_load($file['fid']);
  //       }
  //       manualcrop_croptool_process($element, $form_state, $entity['actions']['ief_entity_crop'], (object) $file, $settings);
  //     }
  //   }
  }

  // if(isset($element['actions'])){
  //   $element['actions']['#attributes'] = array();
  //   $element['actions']['#type'] = 'actions';
  //   if(isset($element['actions']['ief_add'])){
  //     $element['actions']['ief_add']['#attributes']['class'][] = 'secondary';
  //     $element['actions']['ief_add']['#attributes']['class'][] = 'tiny';
  //   }
  //   if(isset($element['actions']['ief_add_existing'])){
  //     $element['actions']['ief_add_existing']['#attributes']['class'][] = 'secondary';
  //     $element['actions']['ief_add_existing']['#attributes']['class'][] = 'tiny';
  //   }
  // }
}
