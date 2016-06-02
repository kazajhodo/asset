<?php

/**
 * @file
 * Provides info about the user entity.
 */

/**
 * Implements hook_entity_property_info_alter() on top of asset module.
 *
 * @see entity_entity_property_info_alter()
 */
function asset_entity_property_info_alter(&$info) {
  $properties = &$info['asset']['properties'];
  $properties['title']['setter callback'] = 'entity_property_verbatim_set';
  $properties['uid']['setter callback'] = 'entity_property_verbatim_set';
  $properties['status']['setter callback'] = 'entity_property_verbatim_set';
  $properties['status']['getter callback'] = 'asset_metadata_entity_get_status';
}

function asset_metadata_entity_get_status($entity, array $options, $name, $entity_type) {
  return $entity->status ? '<i class="fa fa-circle"></i>' : '<i class="fa fa-circle-o"></i>';
}
