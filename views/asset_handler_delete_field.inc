<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying deletes links
 * as fields
 */


class asset_handler_delete_field extends asset_handler_link_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
  }


  function render($values) {
    $type = $values->{$this->aliases['type']};

    //Creating a dummy asset to check access against
    $dummy_asset = (object) array('type' => $type);
    if (!asset_access('edit', $dummy_asset)) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    $id = $values->{$this->aliases['id']};

    return l($text, 'admin/content/assets/asset/' . $id . '/delete');
  }
}
