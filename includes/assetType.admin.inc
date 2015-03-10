<?php

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


    // Change the add page menu to multiple types of entities
    $items[$this->path . '/settings'] = array(
      'title' => 'Settings',
      'description' => 'Add a new asset',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('asset_settings_form'),
      'access arguments' => array('administer asset types'),
      'type' => MENU_LOCAL_TASK,
      'file' => 'assetType.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );
    return $items;
  }

  /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = $additional_header;
    array_unshift($header, t('Label'));
    array_unshift($header, t('Icon'));
    if (!empty($this->entityInfo['exportable'])) {
      $header[] = t('Status');
    }
    $header[] = t('Operations');
    return $header;
  }

  /**
   * Generates the row for the passed entity and may be overridden in order to
   * customize the rows.
   *
   * @param $additional_cols
   *   Additional columns to be added after the entity label column.
   */
  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    $entity_uri = entity_uri($this->entityType, $entity);

    $row[] = !empty($entity->data['icon']) ? '<i class="fa fa-'.$entity->data['icon'].'"></i>' : '';
    $row[] = array(
      'data' => array(
        '#theme' => 'entity_ui_overview_item',
        '#label' => entity_label($this->entityType, $entity),
        '#name' => !empty($this->entityInfo['exportable']) ? entity_id($this->entityType, $entity) : FALSE,
        '#url' => $entity_uri ? $entity_uri : FALSE,
        '#entity_type' => $this->entityType,
      ),
    );

    // Add in any passed additional cols.
    foreach ($additional_cols as $col) {
      $row[] = $col;
    }

    // Add a row for the exportable status.
    if (!empty($this->entityInfo['exportable'])) {
      $row[] = array('data' => array(
          '#theme' => 'entity_status',
          '#status' => $entity->{$this->statusKey},
        ));
    }
    // In case this is a bundle, we add links to the field ui tabs.
    $field_ui = !empty($this->entityInfo['bundle of']) && entity_type_is_fieldable($this->entityInfo['bundle of']) && module_exists('field_ui');
    // For exportable entities we add an export link.
    $exportable = !empty($this->entityInfo['exportable']);
    // If i18n integration is enabled, add a link to the translate tab.
    $i18n = !empty($this->entityInfo['i18n controller class']);

    $ops = array();

    // Add operations depending on the status.
    if (entity_has_status($this->entityType, $entity, ENTITY_FIXED)) {
      $row[] = array(
        'data' => l(t('clone'), $this->path . '/manage/' . $id . '/clone'),
        'colspan' => $this->operationCount(),
      );
    }
    else {
      $name = t('Edit');
      $options = array();
      $this->icon($name, $options);
      $ops[] = l($name, $this->path . '/manage/' . $id, $options);

      if ($field_ui) {
        $name = t('Manage fields');
        $options = array();
        $this->icon($name, $options);
        $ops[] = l($name, $this->path . '/manage/' . $id . '/fields', $options);
        $name = t('Manage display');
        $options = array();
        $this->icon($name, $options);
        $ops[] = l($name, $this->path . '/manage/' . $id . '/display', $options);
      }
      if ($i18n) {
        $name = t('Translate');
        $options = array();
        $this->icon($name, $options);
        $ops[] = l($name, $this->path . '/manage/' . $id . '/translate', $options);
      }
      if ($exportable) {
        $name = t('Clone');
        $options = array();
        $this->icon($name, $options);
        $ops[] = l($name, $this->path . '/manage/' . $id . '/clone', $options);
      }

      if (empty($this->entityInfo['exportable']) || !entity_has_status($this->entityType, $entity, ENTITY_IN_CODE)) {
        $name = t('Delete');
        $options = array();
        $this->icon($name, $options);
        $ops[] = l($name, $this->path . '/manage/' . $id . '/delete', $options += array('query' => drupal_get_destination()));
      }
      elseif (entity_has_status($this->entityType, $entity, ENTITY_OVERRIDDEN)) {
        $name = t('Revert');
        $options = array();
        $this->icon($name, $options);
        $ops[] = l($name, $this->path . '/manage/' . $id . '/revert', $options += array('query' => drupal_get_destination()));
      }
      else {
        $ops[] = '';
      }
    }
    if ($exportable) {
      $name = t('Export');
      $options = array();
      $this->icon($name, $options);
      $ops[] = l($name, $this->path . '/manage/' . $id . '/export', $options);
    }
    $row[] = implode('&nbsp;&nbsp;&nbsp;', $ops);
    return $row;
  }

  protected function icon(&$title, &$options = array()){
    if(function_exists('fett_icon_link')){
      fett_icon_link($title, $options, TRUE, TRUE);
    }
  }
}


/**
 * Generates the asset type editing form.
 */
function asset_type_form($form, &$form_state, $asset_type, $op = 'edit') {

  if ($op == 'clone') {
    $asset_type->name .= ' (cloned)';
    $asset_type->type = '';
  }

  $form['name'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $asset_type->name,
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
      'source' => array('name'),
    ),
    '#description' => t('A unique machine-readable name for this asset type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['plural'] = array(
    '#title' => t('Plural'),
    '#type' => 'textfield',
    '#default_value' => empty($asset_type->plural) ? '' : $asset_type->plural,
    '#description' => t('The human-readable plural name of this asset type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['data']['#tree'] = TRUE;

  $form['data']['auto_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide the title field'),
    '#description' => t('Do not allow the title field to be edited. The title will be automatically generated.'),
    '#default_value' => empty($asset_type->data['auto_title']) ? 0 : $asset_type->data['auto_title'],
  );

  $form['data']['purge'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove when not used'),
    '#description' => t('Automatically purge assets of this type when they are not used anywhere in the site.'),
    '#default_value' => empty($asset_type->data['purge']) ? 0 : $asset_type->data['purge'],
  );

  $form['data']['no_multiple'] = array(
    '#type' => 'checkbox',
    '#title' => t('Prevent multiple use'),
    '#description' => t('Do not allow assets of this type to be re-used. This will prevent using a single asset of this type in different places.'),
    '#default_value' => empty($asset_type->data['no_multiple']) ? 0 : $asset_type->data['no_multiple'],
  );

  $form['data']['require_instance'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require instance'),
    '#description' => t('This will require an instance to be created when creating an asset.'),
    '#default_value' => empty($asset_type->data['require_instance']) ? 0 : $asset_type->data['require_instance'],
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
    '#attributes' => array('class'=>array('clearfix', 'field-inline')),
  );

  // Add widget configuration.
  $form['data']['widget'] = asset_type_widget_form($asset_type, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save asset type'),
    '#weight' => 40,
  );
  return $form;
}

/**
 * Form API validate callback for the asset form
 */
function asset_type_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  // Clean up empty values.
  foreach($values['data'] as $key => $value){
    if(empty($value)) unset($values['data'][$key]);
  }
}

/**
 * Form API submit callback for the type form.
 */
function asset_type_form_submit(&$form, &$form_state) {
  views_invalidate_cache();
  $asset_type = entity_ui_form_submit_build_entity($form, $form_state);
  $asset_type->save();
  $form_state['redirect'] = 'admin/structure/assets';
}

/**
 * Form API submit callback for the delete button.
 */
function asset_type_form_submit_delete(&$form, &$form_state) {
  views_invalidate_cache();
  $form_state['redirect'] = 'admin/structure/assets/manage/' . $form_state['asset_type']->type . '/delete';
}

/**
 * Widget settings form.
 */
function asset_type_widget_form(AssetType $asset_type, &$form_state, $parents = array('data','widget')){
  $form = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Instance options'),
    '#description' => t('An asset can have additional settings. These settings allow changing an asset per implementation.'),
    '#element_validate' => array('asset_type_widget_form_validate'),
  );
  foreach($asset_type->widgetLoad() as $widget_id => $widget){
    $form_state['widgets'][$widget_id] = $widget;
    $form[$widget->getId()] = asset_widget_settings_form_element($widget, $parents);
  }
  return $form;
}

function asset_type_widget_form_validate($element, &$form_state){
  $values = &$form_state['values'];
  $widgets = $form_state['widgets'];
  if(!empty($values['data']['widget'])){
    foreach($values['data']['widget'] as $widget_id => $data){
      if(empty($data['status']) || !isset($widgets[$widget_id])){
        unset($values['data']['widget'][$widget_id]);
      }
      else{
        // Send to widget for validation of values.
        $widget = $widgets[$widget_id];
        $widget->settingsFormValidate($values['data']['widget'][$widget_id]);
      }
    }
  }
}

function asset_widget_settings_form_element($widget, $parents){
  $form = array();
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => $widget->getPluginValue('title'),
    '#default_value' => $widget->getStatus(),
    '#description' => $widget->getPluginValue('description'),
  );
  $parents[] = $widget->getId();
  if($settings_form = $widget->settingsForm($parents)){
    $parents[] = 'status';
    $key = asset_parents_to_name($parents);
    $form['settings'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => $widget->getPluginValue('title') . ' ' . t('Settings'),
    );
    $form['settings']['#states'] = array(
      'visible' => array(
        ':input[name="'.$key.'"]' => array('checked' => TRUE),
      ),
    );
    $form['settings'] += $settings_form;
  }
  return $form;
}

/**
 * Global settings form for assets.
 */
function asset_settings_form($form, &$form_state){
  form_load_include($form_state, 'inc', 'asset', 'includes/asset.fieldAdmin');
  $form['asset'] = asset_configuration_form(variable_get('asset', array()));
  $form['asset']['#title'] = 'Asset Defaults';
  $form['asset']['#tree'] = TRUE;
  return system_settings_form($form);
}
