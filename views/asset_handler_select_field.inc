<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying links to entities
 * as fields.
 */

class asset_handler_select_field extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['id'] = 'id';
    $this->additional_fields['type'] = 'type';
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['text'] = array('default' => '', 'translatable' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    // View mode can be passed to the view as an argument from within asset_browser().
    $view_mode = !empty($this->view->asset_view_mode) ? $this->view->asset_view_mode : NULL;
    $bundle = entity_load('asset_type', array($values->asset_type));
    if(!empty($this->view->asset_js_load)){
      $asset = asset_load_js($values->id, NULL, $view_mode);
    }
    $instance_enabled = 0;
    $output = '<a id="asset-select-'.$values->id.'" class="asset-select" href="#" data-instance="'.$instance_enabled.'" data-id="'.$values->id.'"><span><i class="fa fa-plus"></i></span></a>';
    return $output;
  }
}
