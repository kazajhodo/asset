<?php

/**
 * @file
 * Contains the node from URL argument default plugin.
 */

/**
 * Default argument plugin to extract a node via menu_get_object
 *
 * This plugin actually has no options so it odes not need to do a great deal.
 */
class asset_plugin_argument_default_asset extends views_plugin_argument_default {
  function get_argument() {
    $asset = asset_menu_get_object();
    if (!empty($asset)) {
      return $asset->id;
    }

    // if (arg(0) == 'node' && is_numeric(arg(1))) {
    //   return arg(1);
    // }
  }
}
