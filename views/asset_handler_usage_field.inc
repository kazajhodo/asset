<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying links to entities
 * as fields.
 */

class asset_handler_usage_field extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['id'] = 'id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $asset_id = $values->{$this->aliases['id']};
    $usage = asset_usage($asset_id);
    return isset($usage['total']) ? $usage['total'] : 0;
  }
}
