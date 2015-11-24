<?php


/**
 * The Controller for Asset entities
 */
class AssetTypeController extends EntityAPIControllerExportable {
  public function __construct($entityType) {
    parent::__construct($entityType);
  }

  protected function dataDefaults(){
    return array(
      'icon'                => '',
      'viewable'            => 0,
      'pathauto'            => 0,
      'path'                => '',
      'title_label'         => '',
      'auto_title'          => 0,
      'auto_title_pattern'  => '',
      'render_title'        => 0,
      'allow_status'        => 0,
      'usage_info'          => 0,
      'instance_info'       => 0,
      'purge'               => 0,
      'no_multiple'         => 0,
      'require_instance'    => 0,
    );
  }

   /**
   * Create a asset type - we first set up the values that are specific
   * to our asset type schema but then also go through the EntityAPIController
   * function.
   *
   * @param $type
   *   The machine-readable type of the asset.
   *
   * @return
   *   A asset type object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our Asset
    $values += array(
      'type'   => '',
      'title'  => '',
      'plural' => '',
      'data'   => $this->dataDefaults(),
      'is_new' => TRUE,
    );
    $asset_type = parent::create($values);
    return $asset_type;
  }

  /**
   * When an asset it loaded we want to pull in an asset_instance along with it.
   * We first check to see if there is a default instance attached to the asset
   * via instance_id. If that does not exists, we create an instance pulling in
   * all the default options.
   */
  public function load($ids = array(), $conditions = array(), $instance_id = NULL) {
    $asset_types = parent::load($ids, $conditions);
    $widgets = asset_get_plugin('widget');
    foreach($asset_types as $asset_id => $type){
      // Load in default values for data.
      $type->data += $this->dataDefaults();
      // Widgets are not always available to the public (ie: placeholder). There
      // are times where we need to easily and quickly know this.
      $type->public = FALSE;
      if(!empty($type->data['widget'])){
        foreach($type->data['widget'] as $widget_id => $settings){
          if(!empty($widgets[$widget_id]['public'])){
            $type->public = TRUE;
          }
        }
      }
    }
    return $asset_types;
  }

}
