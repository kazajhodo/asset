<?php

/**
 * Asset browser.
 *
 * @author JaceRider
 *
 * @param  $type
 *   The bundled asset to load a browser for.
 */
function asset_browser( $asset, $category_tid = NULL, $js = FALSE, $return_commands = FALSE ) {
  $type = $asset->type;

  drupal_add_css(drupal_get_path('module','asset') . '/css/asset.css');

  $output = array();
  $output['#prefix'] = '<div class="asset-browser clearfix">';
  $output['#suffix'] = '</div>';

  // $output['taxonomy'] = array(
  //   '#markup' => asset_categories($type, $category_tid),
  //   '#weight' => -5,
  // );

  $view = views_get_view('assets_browser_' . $type);
  //if(!empty($category_tid)) $view->set_arguments(array($category_tid));
  $output['view']['#markup'] = $view->preview('default');
  $view->destroy();

  $output['status'] = array(
    '#markup' => theme('status_messages'),
    '#weight' => -10,
  );

  // Overwrite core views js to allow for exposed filters to work even when
  // view has been reloaded several times on same page.
  drupal_add_js(drupal_get_path('module','asset') . '/js/asset.ajax_views.js');

  if( $js != 'ajax'){
    return $output;
  }

  $commands = array();
  $commands[] = ajax_command_html('', drupal_render($output));
  if($return_commands){
    return $commands;
  }
  $page = array('#type' => 'ajax', '#commands' => $commands);
  ajax_deliver($page);
  exit;
}
