<?php
/**
 * @file
 * Provides the Views content cache asset plugin.
 */

class views_asset_cache_key_asset extends views_asset_cache_key {
  function options_form($value, &$handler = NULL) {
    return array(
      '#title' => t('Node types'),
      '#description' => t('Checks for new or updated assets of any of the selected types.'),
      '#type' => 'checkboxes',
      '#options' => array_merge($this->asset_type_names(), $this->additional_options_for_arguments($handler->view)),
      '#default_value' => $value,
      '#weight' => -10,
    );
  }

  function content_key($asset, $asset_type) {
    if ($asset_type === 'asset') {
      return $asset->type;
    }
  }

  /**
   * We support using the asset type argument for the view key
   */
  function view_key_from_arguments() {
    return array('views_handler_argument_asset_type');
  }
}
