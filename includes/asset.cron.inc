<?php

/*
* Implements hook_cron().
*/
function asset_cron(){
  $queue = DrupalQueue::get('asset_queue_instance_purge');
  foreach (asset_instance_orphans() as $instance_id) {
    $queue->createItem($instance_id);
  }
  $queue = DrupalQueue::get('asset_queue_purge');
  foreach (asset_orphans() as $asset_id) {
    $queue->createItem($asset_id);
  }
}

/**
* Implements hook_cron_queue_info().
*/
function asset_cron_queue_info() {
  $queues['asset_queue_purge'] = array(
    'worker callback' => 'asset_queue_purge_worker',
    // One second max runtime per cron run.
    'time' => 5,
  );
  $queues['asset_queue_instance_purge'] = array(
    'worker callback' => 'asset_queue_instance_purge_worker',
    // One second max runtime per cron run.
    'time' => 5,
  );
  return $queues;
}

function asset_queue_purge_worker($asset_id){
  if($asset = asset_load($asset_id)){
    watchdog('asset', 'Asset has been purged: %asset_id.', array('%asset_id' => $asset_id));
    $asset->delete();
  }
}

function asset_queue_instance_purge_worker($instance_id){
  if($instance = asset_instance_load($instance_id)){
    watchdog('asset', 'Asset instance has been purged: %instance_id.', array('%instance_id' => $instance_id));
    $instance->delete();
  }
}
