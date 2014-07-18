<?php


/**
 * The Controller for Asset entities
 */
class AssetTypeController extends EntityAPIControllerExportable {
  public function __construct($entityType) {
    parent::__construct($entityType);
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
      'id'     => '',
      'is_new' => TRUE,
      'name'   => '',
      'plural' => '',
      'data'   => '',
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
