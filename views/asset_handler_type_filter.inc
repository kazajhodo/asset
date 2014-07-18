<?php

/**
 * @file
 * Definition of views_handler_filter_asset_type.
 */

/**
 * Filter by asset type.
 *
 * @ingroup views_filter_handlers
 */
class asset_handler_type_filter extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Content types');
      $types = asset_get_types();
      $options = array();
      foreach ($types as $type => $info) {
        $options[$type] = t($info->name);
      }
      asort($options);
      $this->value_options = $options;
    }
  }
}
