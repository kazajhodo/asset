<?php

/**
 * The class used for asset type entities
 */
class AssetType extends Entity {

  public function __construct($values = array()) {
    parent::__construct($values, 'asset_type');
  }

  /**
   * Get all available widgets.
   */
  public function widgetLoad(){
    $widgets = array();
    foreach(asset_get_plugin('widget') as $widget_id => $widget){
      $settings = $this->widgetLoad_settings($widget_id);
      $widgets[$widget_id] = asset_get_handler('widget', $widget_id, $settings);
    }
    return !empty($widgets) ? $widgets : array();
  }

  /**
   * Get enabled widgets
   */
  public function widgetLoadActive(){
    $widgets = array();
    foreach($this->widgetLoad() as $widget_id => $widget){
      if($widget->getStatus()) $widgets[$widget_id] = $widget;
    }
    return $widgets;
  }

  /**
   * Load in widget settings contained in the data table.
   */
  public function widgetLoad_settings($widget_id){
    return empty($this->data['widget'][$widget_id]) ? array() : array_filter($this->data['widget'][$widget_id]);
  }
}
