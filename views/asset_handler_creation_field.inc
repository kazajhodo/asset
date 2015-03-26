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

    $output = $bar = array();
    if($this->view->base_table == 'asset' && isset($this->view->filter['type']->value)){
      $bar['right'] = array(
        '#prefix' => '<ul class="right">',
        '#suffix' => '</ul>',
      );
      foreach($this->view->filter['type']->value as $type){
        $asset_type = asset_type_load($type);
        $link = menu_get_item('admin/content/assets/add/' . $asset_type->type);
        $link['localized_options']['query'] = drupal_get_destination();
        if(function_exists('boushh_action_links_add')){
          boushh_action_links_add($link);
        }
        else{
          $bar['right'][] = array(
            '#theme' => 'menu_local_action__ops',
            '#link' => $link
          );
        }
      }
    }

    if(!empty($bar) && !function_exists('boushh_action_links_add')){
      $output['#prefix'] = '<nav class="ops-bar" data-topbar>';
      $output['#suffix'] = '</nav>';
      $output['title']['#markup'] = '<ul class="title-area"><li class="name"><h1></h1></li><li class="toggle-topbar menu-icon"><a href="#"><span>Actions</span></a></li></ul>';
      $output['bar']['#prefix'] = '<section class="ops-bar-section">';
      $output['bar']['#suffix'] = '</section>';
      $output['bar'] += $bar;
    }

    return render($output);
  }
}
