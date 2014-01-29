<?php

/**
 * @file
 * Abstraction of the selection logic of a Mee instance.
 *
 * Implementations that wish to provide an implementation of this should
 * register it using CTools' plugin system.
 */
interface AssetInstanceHandler {

  public function __construct($type, $args = NULL);

  public function instance_form(&$form, &$form_state, $settings, $defaults);

}

/**
 * An abstract implementation of ButtonsReference_LinksHandler.
 */
abstract class AssetInstanceHandler_Abstract implements AssetInstanceHandler {

  /**
   * The name of the button plugin.
   */
  protected $type;

  /**
   * The plugin definition.
   */
  protected $plugin;

  /**
   * Constructor for the links.
   *
   * @param $type
   * The name of the links plugin.
   */
  public function __construct($type, $args = NULL) {
    $this->type = $type;
    ctools_include('plugins');
    $plugin = ctools_get_plugins('mee', 'instance', $type);
    $this->plugin = $plugin;
  }

  public function defaults($settings) {
    return array();
  }

  public function settings_form(&$form, &$form_state, $defaults, $parent_name) {}

  public function instance_form(&$form, &$form_state, $settings, $defaults) {}

  public function instance_render(&$vars, $settings, $config) {}

  public function instance_render_preview(&$vars, $settings, $config) {
    $this->instance_render($vars, $settings, $config);
  }

}

/**
 * A null implementation of Mee_LinksHandler.
 */
class AssetInstance_InstanceHandler_Broken extends AssetInstanceHandler_Abstract {

}
