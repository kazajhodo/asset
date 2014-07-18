<?php

/**
 * The class used for asset instance entities
 */
class AssetInstance extends Entity {

  public $type;

  public function __construct($values = array()) {
    parent::__construct($values, 'asset_instance');
  }

  protected function defaultLabel() {
    return $this->id;
  }

  /**
   * Prepare an asset instance with widget instances.
   */
  public function widgetPrepare(){
    if(empty($this->widgets)){
      $this->widgetLoad();
    }
  }

  /**
   * Get all available widgets
   */
  public function widgetLoad(){
    // static $widgetLoad;
    // if (!isset($widgetLoad[$this->id]) || empty($this->widgets)) {
    if (empty($this->widgets)) {
      $this->widgets = array();
      $asset_type = asset_type_load($this->type);
      foreach($asset_type->widgetLoadActive() as $widget_id => $widget){
        module_load_include('inc', 'asset', 'plugins/widget/'.$widget_id);
        $values = isset($this->data[$widget_id]) ? $this->data[$widget_id] : $widget->instanceDefaults();
        // Set these default values into the widgets.
        $widget->setValues($values);
        // Set these values to the data object;
        $this->data[$widget_id] = $values;
        // $widget_cache[$this->id][$widget_id] = $widget;
        $this->widgets[$widget_id] = $widget;
      }
      // $widgetLoad[$this->id] = TRUE;
    }
    return $this->widgets;
  }

  /**
   * Returns form api ready widget fields.
   */
  public function widgetFields(){
    $form = array();
    foreach($this->widgetLoad() as $widget_id => $widget){
      $form[$widget_id] = $widget->instanceForm();
    }
    return $form;
  }

  /**
   * Validates form api widget fields.
   */
  public function widgetFieldsValidate(&$values){
    foreach($this->widgetLoad() as $widget_id => $widget){
      if(isset($values[$widget_id])){
        $widget->instanceFormValidate($values[$widget_id]);
      }
    }
  }

}
