<?php

/**
 * @file
 * Definition of views_handler_argument_asset_type.
 */

/**
 * Argument handler to accept a asset type.
 */
class asset_handler_type_argument extends views_handler_argument_string {
  function construct() {
    parent::construct('type');
  }

  /**
   * Override the behavior of summary_name(). Get the user friendly version
   * of the asset type.
   */
  function summary_name($data) {
    return $this->asset_type($data->{$this->name_alias});
  }

  /**
   * Override the behavior of title(). Get the user friendly version of the
   * asset type.
   */
  function title() {
    return $this->asset_type($this->argument);
  }

  function asset_type($type) {
    $output = asset_type_get_name($type);
    if (empty($output)) {
      $output = t('Unknown content type');
    }
    return check_plain($output);
  }
}
