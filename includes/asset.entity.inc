<?php

/**
 * The class used for asset entities
 */
class Asset extends Entity {

  public function __construct($values = array()) {
    parent::__construct($values, 'asset');
  }

  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'asset/' . $this->id);
  }

  /**
   * Get all available widgets
   */
  public function widget_load(){
    $asset_type = asset_type_load($this->type);
    $widgets = array();
    foreach($asset_type->widget_load_active() as $widget_id => $widget){
      $widgets[$widget_id] = $widget;
    }
    return $widgets;
  }

}
