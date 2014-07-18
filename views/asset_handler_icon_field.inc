<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying links to entities
 * as fields.
 */

class asset_handler_icon_field extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['id'] = 'id';
    $this->additional_fields['type'] = 'type';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $asset_type = asset_type_load($values->{$this->aliases['type']});
    return isset($asset_type->data['icon']) ? '<i class="fa fa-'.$asset_type->data['icon'].'"></i>' : '';
  }
}
