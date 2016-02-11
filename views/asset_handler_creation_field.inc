<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying asset creation
 * links
 */


class asset_handler_creation_field extends views_handler_field {

  function query() {
    // do nothing -- to override the parent query.
  }

  function render($values) {
    if($this->view->base_table == 'asset' && isset($this->view->filter['type']->value)){
      foreach($this->view->filter['type']->value as $type){
        $asset_type = asset_type_load($type);
        $link = menu_get_item(asset_url($type, 'create'));
        if($link && $link['access']){
          $link['localized_options']['query'] = drupal_get_destination();
          $link['title'] = 'Add ' . $link['title'];
          $action = array(
            '#theme' => 'menu_local_action__ops',
            '#link' => $link
          );
          asset_menu_action_add($action);
        }
      }
    }
  }
}
