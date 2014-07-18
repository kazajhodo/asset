<?php

/**
 * The Controller for Asset entities
 */
class AssetController extends EntityAPIController {
  public function __construct($assetType) {
    parent::__construct($assetType);
  }

  /**
   * Create a asset - we first set up the values that are specific
   * to our asset schema but then also go through the EntityAPIController
   * function.
   *
   * @param $type
   *   The machine-readable type of the asset.
   *
   * @return
   *   A asset object with all default fields initialized.
   */
  public function create(array $values = array()) {
    global $user;

    // Add values that are specific to our Asset
    $values += array(
      'id'          => '',
      'is_new'      => TRUE,
      'name'        => '',
      'uid'         => $user->uid,
      'created'     => '',
      'changed'     => '',
      'instance_id' => NULL,
      'data'        => '',
    );

    $asset = parent::create($values);
    $asset->instance = asset_instance_create($asset);

    return $asset;
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Add in created and changed times.
    if ($entity->is_new = isset($entity->is_new) ? $entity->is_new : 0){
      $entity->created = time();
    }
    $entity->changed = time();
    $entity->instance_id = !empty($entity->instance_id) ? $entity->instance_id : NULL;
    if (empty($entity->name)){
      $asset_type = asset_type_load($entity->type);
      $entity->name = t('System') . ' [' . $asset_type->name . ']';
    }
    $return = parent::save($entity, $transaction);
    if($return){
      if(isset($entity->instance) && isset($entity->instance_form)){
        $instance = $entity->instance;
        if(!empty($entity->instance_form['status']) && !empty($entity->instance_form['data'])){
          $instance->data = $entity->instance_form['data'];
          $instance->asset_id = $entity->id;
          $instance->save();
          $entity->instance_id = $instance->id;
          unset($entity->instance_form);
          $entity->save();
        }
        elseif(isset($entity->instance_form['status']) && !empty($entity->instance_id)){
          // The default instance will be automatically cleaned if it is not
          // already used somehwere else.
          $entity->instance_id = NULL;
          unset($entity->instance_form);
          $entity->save();
        }
      }
    }
  }

  /**
   * When an asset it loaded we want to pull in an asset_instance along with it.
   * We first check to see if there is a default instance attached to the asset
   * via instance_id. If that does not exists, we create an instance pulling in
   * all the default options.
   */
  public function load($ids = array(), $conditions = array(), $instance_id = NULL) {
    $assets = parent::load($ids, $conditions);
    foreach($assets as $asset_id => $asset){
      $assets[$asset_id] = $this->loadInstance($asset, $instance_id);
    }
    return $assets;
  }

  /**
   * Load an instance into an asset. If no instance is provided, a default one
   * will be created.
   */
  public function loadInstance($asset, $instance_id = NULL){
    static $object_cache;
    // dsm($asset);
    // Load an instance if there is not one currently.
    $load_asset = !isset($asset->instance);
    // If there is an instance but it does not match the instance_id being requested
    $load_asset = $load_asset ? $load_asset : (isset($asset->instance) && !empty($instance_id) && $asset->instance->id != $instance_id);
    // If there is an instance but we are requesting the default
    $load_asset = $load_asset ? $load_asset : (isset($asset->instance) && empty($instance_id) && isset($asset->instance->id) && $asset->instance->id != $asset->instance_id);
    // Pull in the default instance set to this asset.
    if($load_asset){
      if(!empty($instance_id) && $instance = asset_instance_load($instance_id)){
        $asset->instance = $instance;
      }
      elseif(!empty($asset->instance_id) && $instance = asset_instance_load($asset->instance_id)){
        $asset->instance = $instance;
      }
      else{
        $asset->instance = asset_instance_create($asset);
      }
      if(!empty($asset->instance->id)){
        // Set the currently loaded instance id on the asset if it is set.
        $asset->instance_id = $asset->instance->id;
      }
    }
    // There are times when the same asset is loaded multiple times with
    // different instances. Each time this happens we need to clone the
    // entity.
    $key = $asset->id . '-' . $asset->instance->id;
    if (!isset($object_cache[$key])) {
      $asset = clone $asset;
      $object_cache[$key] = $asset;
    }
    else{
      $object_cache[$key] = $asset;
    }
    return $object_cache[$key];
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    parent::delete($ids, $transaction);
    // When deleting an asset, also delete instances assigned to it.
    foreach($ids as $id){
      foreach(asset_instance_load_by_asset($id) as $instance){
        $instance->delete();
      }
      // Remove all usage for this asset.
      asset_usage_purge_by_asset($id);
    }
  }

  // /**
  //  * Overridden.
  //  * @see DrupalDefaultEntityController::cacheSet()
  //  */
  // public function cacheSet($entities) {
  //   dsm($entities);
  //   parent::cacheSet($entities);
  // }

}
