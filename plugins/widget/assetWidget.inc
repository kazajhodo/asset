<?php

/**
 * @file
 * Abstraction of the selection logic of an asset.
 *
 * Implementations that wish to provide an implementation of this should
 * register it using CTools' plugin system.
 */
interface assetWidgetInterface {

  public function __construct($type, $args = NULL);

  // public function instance_form(&$form, &$form_state, $settings, $defaults);

}

/**
 * An abstract implementation of assetWidgetInterface.
 */
abstract class assetWidget implements assetWidgetInterface {

  /**
   * The name of the button plugin.
   */
  protected $type;

  /**
   * The plugin definition.
   */
  protected $plugin;

  /**
   * Widget settings.
   */
  protected $settings;

  /**
   * Widget value.
   */
  protected $values;

  /**
   * Constructor for the links.
   *
   * @param $type
   * The name of the links plugin.
   */
  public function __construct($type, $args = NULL) {
    $this->type = $type;
    ctools_include('plugins');
    $plugin = ctools_get_plugins('asset', 'widget', $type);
    $this->plugin = $plugin;
    if(isset($args[0])){
      $this->settings = $args[0];
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Getters
  //////////////////////////////////////////////////////////////////////////////

  public function getGlobal(){
    return !empty($this->plugin['global']);
  }

  public function getId(){
    return $this->getPluginValue('name');
  }

  public function getPluginValue($key){
    return isset($this->plugin[$key]) ?  $this->plugin[$key] : '';
  }

  public function getStatus(){
    $settings = $this->getSettings();
    return !empty($settings['status']) && $settings['status'] === 1 ? TRUE : FALSE;
  }

  public function getSettings(){
    return $this->settings;
  }

  public function getValues(){
    return $this->values + $this->getDefaults();
  }

  public function getDefaults(){
    return $this->instanceDefaults();
  }

  public function getFormSettings(){
    $settings = $this->getSettings();
    return isset($settings['settings']) ? $settings['settings'] : array();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Setters
  //////////////////////////////////////////////////////////////////////////////

  public function setValues($values){
    $this->values = $values;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Methods
  //////////////////////////////////////////////////////////////////////////////

  public function instanceDefaults() {
    return array();
  }

  public function settingsForm($parents) {}

  public function settingsFormValidate() {}

  public function instanceForm() {}

  public function viewMode(&$view_mode) {}

  public function instanceFormValidate(&$values) {}

  public function instanceFormSubmit(&$values) {}

  public function instanceRender(&$vars) {}

  public function instanceRenderPreview(&$vars) {
    $this->instanceRender($vars);
  }

}

/**
 * A null implementation of assetWidget.
 */
class assetWidgetBroken extends assetWidget {

}
