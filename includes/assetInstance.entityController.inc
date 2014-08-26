<?php

/**
 * The Controller for Asset entities
 */
class AssetInstanceController extends EntityAPIController {

  public function __construct($entityType) {
    parent::__construct($entityType);
  }

   /**
   * Create a asset instance - we first set up the values that are specific
   * to our asset instance schema but then also go through the EntityAPIController
   * function.
   *
   * @param $type
   *   The machine-readable type of the asset.
   *
   * @return
   *   A asset instance object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our Asset
    $values += array(
      'id'        => 0,
      'is_new'    => TRUE,
      'type'      => $values['asset']->type,
      'asset_id' => $values['asset']->id,
      'data'      => '',
    );
    // Clean up.
    unset($values['asset']);
    $asset_instance = parent::create($values);
    // Load in widgets.
    $asset_instance->widgetPrepare();
    return $asset_instance;
  }

  /**
   * Overridden.
   * @see DrupalDefaultEntityController#load($ids, $conditions)
   *
   * In contrast to the parent implementation we factor out query execution, so
   * fetching can be further customized easily.
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = parent::load($ids, $conditions);
    foreach($entities as $entity_id => $entity){
      $entity->widgetPrepare();
    }
    return $entities;
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $entity->changed = time();
    return parent::save($entity, $transaction);
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
      // Remove all usage for this instance.
      asset_usage_purge_by_instance($id);
    }
    // Clear usage.
    asset_usage_clear();
  }

}
