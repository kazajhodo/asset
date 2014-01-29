<?php

/**
 * Defines a line item summary area handler so the summary can be plugged into
 *   the View itself.
 */
class asset_handler_create_link_area extends views_handler_area {

  function option_definition() {
    $options = parent::option_definition();

    // Define an option to control the links displayed in the summary.
    $options['assets'] = array('default' => array());

    foreach (asset_get_types() as $type) {
      $options['assets']['default'][$type->type] = 0;
    }

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Add checkboxes for the summary links if any are available.
    $options = array();

    foreach (asset_get_types() as $type) {
      $options[$type->type] = $type->label;
    }

    if (!empty($options)) {
      $form['assets'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Assets'),
        '#description' => t('Select the links you want to appear beneath the summary.'),
        '#options' => $options,
        '#default_value' => $this->options['assets'],
      );
    }
  }

  function render($empty = FALSE) {

    $output = '';
    if(!empty($this->options['assets'])){
      $links = array();
      $info = entity_get_info('asset');
      foreach (asset_get_types() as $type) {
        if ($this->options['assets'][$type->type]) {
          $links[] = l('<i class="fa fa-' . $type->data['icon'] . '"></i> Create New ' . $type->label, 'admin/content/assets/add/' . $type->type, array('html' => TRUE, 'attributes' => array('class'=>array('button')), 'query' => drupal_get_destination()));
        }
      }
      $output .= implode(' ', $links);
    }
    return $output;
  }
}
