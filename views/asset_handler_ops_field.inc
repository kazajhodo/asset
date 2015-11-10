<?php

/**
 * This field handler aggregates operations that can be done on a asset
 * under a single field providing a more flexible way to present them in a view
 */
class asset_handler_ops_field extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['id'] = 'id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $id = $this->get_value($values, 'id');
    $links = menu_contextual_links('asset', asset_url($id, 'list'), array($id));
    if (!empty($links)) {
      foreach($links as &$link){
        if(isset($_GET['destination'])){
          $link['query']['destination'] = $_GET['destination'];
        }
        elseif($destination = $this->view->get_url()){
          $link['query']['destination'] = $destination;
        }
        else{
          $link['query'] = drupal_get_destination();
        }
        if(function_exists('fawesome_text') && $text = fawesome_text($link['title'])){
          $link['title'] = $text;
          $link['html'] = true;
        }
      }
      return theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline', 'operations'))));
    }
  }
}
