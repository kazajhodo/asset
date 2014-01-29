<?php

/**
 * @file
 * Asset type editing UI.
 */

/**
 * UI controller.
 */
class AssetInstanceUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage asset entity instancess, including adding
    and removing fields and the display of fields.';
    return $items;
  }

  /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = $additional_header;
    array_unshift($header, t('ID'));
    $header[] = t('Asset Type');
    if (!empty($this->entityInfo['exportable'])) {
      $header[] = t('Status');
    }
    // Add operations with the right colspan.
    $header[] = array('data' => t('Operations'), 'colspan' => $this->operationCount());
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

    $row[] = array('data' => array(
      '#theme' => 'entity_ui_overview_item',
      '#label' => entity_label($this->entityType, $entity),
      '#name' => !empty($this->entityInfo['exportable']) ? entity_id($this->entityType, $entity) : FALSE,
      '#url' => $entity_uri ? $entity_uri : FALSE,
      '#entity_type' => $this->entityType),
    );

    $row[] = $entity->type;

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

    // Add operations depending on the status.
    if (entity_has_status($this->entityType, $entity, ENTITY_FIXED)) {
      $row[] = array('data' => l(t('clone'), $this->path . '/manage/' . $id . '/clone'), 'colspan' => $this->operationCount());
    }
    else {
      $row[] = l(t('edit'), $this->path . '/manage/' . $id);

      if ($field_ui) {
        $row[] = l(t('manage fields'), $this->path . '/manage/' . $id . '/fields');
        $row[] = l(t('manage display'), $this->path . '/manage/' . $id . '/display');
      }
      if ($i18n) {
        $row[] = l(t('translate'), $this->path . '/manage/' . $id . '/translate');
      }
      if ($exportable) {
        $row[] = l(t('clone'), $this->path . '/manage/' . $id . '/clone');
      }

      if (empty($this->entityInfo['exportable']) || !entity_has_status($this->entityType, $entity, ENTITY_IN_CODE)) {
        $row[] = l(t('delete'), $this->path . '/manage/' . $id . '/delete', array('query' => drupal_get_destination()));
      }
      elseif (entity_has_status($this->entityType, $entity, ENTITY_OVERRIDDEN)) {
        $row[] = l(t('revert'), $this->path . '/manage/' . $id . '/revert', array('query' => drupal_get_destination()));
      }
      else {
        $row[] = '';
      }
    }
    if ($exportable) {
      $row[] = l(t('export'), $this->path . '/manage/' . $id . '/export');
    }
    return $row;
  }


}

function asset_instance_form_fields(&$form, $instance, $instance_type){
  $active = asset_instance_get_active($instance_type);
  $plugins = asset_instance_get_plugin('instance');
  foreach($active as $instance_type => $settings){
    $handler = asset_instance_get_handler('instance', $instance_type);
    $plugin = $plugins[$instance_type];
    if(!empty($plugin['css_admin'])){
      $form['data']['#attached']['css'][] = drupal_get_path('module', $instance['module']) . '/' . $instance['css_admin'];
    }
    $handler->instance_form($form, $form_state, $settings, $instance->data);
  }
  return count($form) > 1 ? TRUE : FALSE;
}

/**
 * Generates the asset type editing form.
 */
function asset_instance_form($form, &$form_state, $asset_instance, $op = 'edit') {

  if ($op == 'clone') {
    $asset_instance->label .= ' (cloned)';
    $asset_instance->type = '';
  }
  // Machine-readable type name.
  $types = asset_get_types();
  $options = array();
  foreach($types as $type){
    $options[$type->type] = $type->label;
  }

  $form['#id'] = 'asset-instance-form';
  $form_state['asset_instance'] = $asset_instance;

  if(!empty($asset_instance->lock)){
    $form['type'] = array(
      '#type' => 'hidden',
      '#value' => $asset_instance->type,
    );
  }
  else{
    $form['type'] = array(
      '#title' => t('Asset type of this instance'),
      '#type' => 'select',
      '#required' => TRUE,
      '#default_value' => $asset_instance->type,
      '#options' => $options,
      '#submit' => array('asset_instance_form_type_submit'),
      '#ajax' => array(
        'callback' => 'asset_instance_form_type_submit_ajax',
        'wrapper' => 'asset-instance-form',
        'method' => 'replace',
        'effect' => 'none',
      ),
    );
  }

  $form['data'] = array();
  $form['data']['#tree'] = TRUE;

  if(isset($asset_instance->type) || isset($form_state['values']['type'])){
    $instance_type = isset($form_state['values']['type']) ? $form_state['values']['type'] : $asset_instance->type;
    $has_instance = asset_instance_form_fields($form['data'], $asset_instance, $instance_type);
  }

  $form['#submit'][] = 'asset_instance_form_submit';
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 40,
  );

  if(empty($has_instance)){
    $form = array();
    drupal_set_message(t('There are no settings for this asset.'),'warning');
  }

  return $form;
}

function asset_instance_form_type_submit_ajax($form, &$form_state){
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function asset_instance_form_submit(&$form, &$form_state) {
  $asset_instance = entity_ui_form_submit_build_entity($form, $form_state);
  $asset_instance->save();
  $form_state['redirect'] = 'admin/structure/asset_instances';
}

/**
 * Form API submit callback for the delete button.
 */
function asset_instance_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/asset_instances/manage/' . $form_state['asset_instance']->type . '/delete';
}
