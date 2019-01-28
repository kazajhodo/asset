<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_image_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  if($fields = asset_entityreference_contains($field, 'image')){
    $field_names = array();
    foreach($fields as $key => $target){
      $target['parents'][] = $target['field_name'];
      $field_names[$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    $element['field_name'] = array(
      '#title' => t('Target image field'),
      '#type' => 'select',
      '#default_value' => $settings['field_name'],
      '#required' => TRUE,
      '#options' => $field_names,
      '#weight' => -100,
    );
  }
  if($fields = asset_entityreference_contains($field, 'video_embed_field')){
    $field_names = array();
    foreach($fields as $key => $target){
      $target['parents'][] = $target['field_name'];
      $field_names[$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    $element['field_video_name'] = array(
      '#title' => t('Target video field'),
      '#type' => 'select',
      '#default_value' => $settings['field_name'],
      '#required' => TRUE,
      '#options' => $field_names,
      '#weight' => -99,
    );
    $video_styles = video_embed_field_video_style_options(TRUE);
    $video_styles[NULL] = t('Display as image');
    $element['video_style'] = array(
      '#title' => t('Video style'),
      '#type' => 'select',
      '#default_value' => $settings['video_style'],
      '#options' => $video_styles,
    );
  }
  if(!empty($element)){
    $elid   = drupal_html_id('asset_image_as_url');
    module_load_include('inc', 'image', 'image.field');
    $element += image_field_formatter_settings_form($field, $instance, $view_mode, $form, $form_state);

    $element['image_link']['#weight'] = 100;
    $element['image_link']['#id'] = $elid . '-link';
    $element['image_link']['#states'] = array(
      'visible' => array(
        '#' . $elid => array('checked' => FALSE),
      ),
    );
    $links = asset_field_contains($field, $instance, 'link_field');
    foreach($links as $key => $target){
      $target['parents'][] = $target['field_name'];
      $element['image_link']['#options'][$key] = $target['instance']['label'] . ' [' . implode($target['parents'], ' > ') . ']';
    }
    if (module_exists('file_force')) {
      $element['force_download'] = array(
        '#type' => 'checkbox',
        '#title' => t('Force Download'),
        '#default_value' => $settings['force_download'],
        '#weight' => 101,
        '#states' => array(
          'visible' => array(
            '#' . $elid . '-link' => array('value' => 'file'),
          ),
        ),
      );
    }
    $element['as_url'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display as URL'),
      '#default_value' => $settings['as_url'],
      '#id' => $elid,
    );
    $element['as_bg'] = array(
      '#type' => 'checkbox',
      '#title' => t('Set image as a background within an element'),
      '#default_value' => $settings['as_bg'],
      '#id' => $elid . '-bg',
    );
    $element['as_bg_class'] = array(
      '#type' => 'textfield',
      '#title' => t('Classes to add to background element'),
      '#default_value' => $settings['as_bg_class'],
      '#states' => array(
        'visible' => array(
          '#' . $elid . '-bg' => array('checked' => TRUE),
        ),
      ),
    );
    $element['limit'] = array(
      '#type' => 'textfield',
      '#title' => t('Limit the number of items to display'),
      '#default_value' => $settings['limit'],
      '#id' => $elid . '-limit',
    );
    $element['random'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display in random order'),
      '#default_value' => $settings['random'],
      '#id' => $elid . '-random',
    );

    if(module_exists('colorbox')){
      $element['image_link']['#states']['visible']['#' . $elid . '-colorbox'] = array('checked' => FALSE);
      $element['image_style']['#states']['visible']['#' . $elid . '-colorbox'] = array('checked' => FALSE);
      $element['as_url']['#states']['visible']['#' . $elid . '-colorbox'] = array('checked' => FALSE);
      $element['colorbox'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use Colorbox'),
        '#default_value' => $settings['colorbox'],
        '#id' => $elid . '-colorbox',
      );
      $colorbox_elements = colorbox_field_formatter_settings_form($field, $instance, $view_mode, $form, $form_state);
      if(isset($element['video_style'])){
        $colorbox_elements['colorbox_video_only'] = array(
          '#type' => 'checkbox',
          '#title' => t('Colorbox should only be used on videos'),
          '#default_value' => $settings['colorbox_video_only'],
        );
      }
      foreach($colorbox_elements as &$el){
        $el['#states']['visible']['#' . $elid . '-colorbox'] = array('checked' => TRUE);
      }
      $colorbox_elements['colorbox_gallery']['#options']['asset-delta'] = t('Base Asset Delta');
      $element += $colorbox_elements;
    }
  }
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_image_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();
  module_load_include('inc', 'image', 'image.field');
  $label = 'NONE';
  if(($settings['field_name'] && $fields = asset_entityreference_contains($field, 'image')) && isset($fields[$settings['field_name']])){
    $target = $fields[$settings['field_name']];
    $target['parents'][] = $target['field_name'];
    $label = implode($target['parents'], ' > ');
  }
  $summary = t('Target field: @field', array('@field' => $label));
  if(($settings['field_video_name'] && $fields = asset_entityreference_contains($field, 'video_embed_field')) && isset($fields[$settings['field_video_name']])){
    $target = $fields[$settings['field_video_name']];
    $target['parents'][] = $target['field_name'];
    $label = implode($target['parents'], ' > ');
    $summary .= '<br>' . t('Target video field: @field', array('@field' => $label));
    $video_styles = video_embed_field_video_style_options(FALSE);
    $summary .= '<br>' . t('Video style: @field', array('@field' => $settings['video_style'] ? $video_styles[$settings['video_style']] : 'Thumbnail'));
  }
  if(module_exists('colorbox') && $settings['colorbox']){
    $colorbox_settings = &$instance['display'][$view_mode];
    if(substr($colorbox_settings['settings']['colorbox_gallery'],0,5) == 'asset'){
      $colorbox_settings['settings']['colorbox_gallery'] = 'custom';
      $colorbox_settings['settings']['colorbox_gallery_custom'] = t('Base Asset Delta');
    }
    $summary .= '<br>' . t('As Colorbox: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
    if($settings['colorbox_video_only']){
      $summary .= '<br>' . t('Colorbox for video only: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
    }
    $summary .= '<br>' . colorbox_field_formatter_settings_summary($field, $instance, $view_mode);
  }else{
    $summary .= '<br>' . image_field_formatter_settings_summary($field, $instance, $view_mode);
    if($settings['as_url']){
      $summary .= '<br>' . t('As URL: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
    }
    if($settings['image_link'] && !in_array($settings['image_link'], array('content','file'))){
      $fields = asset_field_contains($field, $instance, 'link_field');
      if(isset($fields[$settings['image_link']])){
        $target = $fields[$settings['image_link']];
        $target['parents'][] = $target['field_name'];
        $label = implode($target['parents'], ' > ');
        $summary .= '<br>' . t('Linked to: @field', array('@field' => $label));
      }
    }
    if (module_exists('file_force') && $settings['image_link'] && $settings['image_link'] == 'file' && $settings['force_download']) {
      $summary .= '<br>' . t('Force download: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
    }
  }
  if($settings['as_bg']){
    $summary .= '<br>' . t('As Background: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
  }
  if($settings['limit']){
    $summary .= '<br>' . t('Limit to: !limit', array('!limit' => format_plural($settings['limit'], '1 item', '@count items')));
  }
  if($settings['random']){
    $summary .= '<br>' . t('Random: !icon', array('!icon' => '<i class="fa fa-check text-success"></i>'));
  }
  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_image_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $settings = $display['settings'];
  $element = array();
  $use_colorbox = !empty($settings['colorbox']) && module_exists('colorbox');

  // Nested link field support
  if (!$use_colorbox && !in_array($settings['image_link'], array('content','file'))) {
    $link_fields = asset_field_contains($field, $instance, 'link_field');
    if(isset($link_fields[$settings['image_link']])){
      $link_target = $link_fields[$settings['image_link']];
      $target_link_entity_wrapper = asset_entityreference_get($entity_type, $entity, $link_target);
    }
  }

  foreach($items as $delta => $item){

    if($use_colorbox){
      $once = FALSE;
    }

    if(($settings['field_name'] && $image_fields = asset_entityreference_contains($field, 'image')) && isset($image_fields[$settings['field_name']])){
      $image_target = $image_fields[$settings['field_name']];
      $target_image_entity_wrappers = asset_entityreference_get($entity_type, $entity, $image_target, $delta);
      foreach($target_image_entity_wrappers as $child_delta => $target_entity_wrapper){
        $target_field = $image_target['field'];
        $target_instance = $image_target['instance'];
        $target_entity_type = $target_entity_wrapper->type();
        $target_entity = $target_entity_wrapper->value();
        if(!$target_entity) continue;

        $target_items = field_get_items($target_entity_type, $target_entity, $image_target['field_name']);
        if($target_items){
          module_load_include('inc', 'image', 'image.field');

          if($use_colorbox && empty($settings['colorbox_video_only'])){
            $display_colorbox = $display;
            if(substr($display_colorbox['settings']['colorbox_gallery'],0,5) == 'asset'){
              $display_colorbox['settings']['colorbox_gallery'] = 'custom';
              $display_colorbox['settings']['colorbox_gallery_custom'] = 'asset-' . $delta;
            }
            if(!$once){
              $once = TRUE;
            }
            else{
              $display_colorbox['settings']['colorbox_node_style_first'] = $display_colorbox['settings']['colorbox_node_style'];
            }
            $element[$delta][$child_delta] = colorbox_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display_colorbox);
          }
          else{
            $element[$delta][$child_delta] = image_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display);
          }

          if($settings['as_bg']){
            foreach($element[$delta] as &$el){
              $el['#original_theme'] = $el['#theme'];
              $el['#theme'] = 'asset_image_bg';
              $el['#attributes']['class'][] = $settings['as_bg_class'];
            }
          }
        }

        // if($target_items){
        //   module_load_include('inc', 'image', 'image.field');
        //   $element[$delta][$child_delta] = image_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display);
        // }
      }
    }

    if(($settings['field_video_name'] && $video_fields = asset_entityreference_contains($field, 'video_embed_field')) && isset($video_fields[$settings['field_video_name']])){
      $video_target = $video_fields[$settings['field_video_name']];
      $target_video_entity_wrappers = asset_entityreference_get($entity_type, $entity, $video_target, $delta);
      foreach($target_video_entity_wrappers as $child_delta => $target_entity_wrapper){
        $target_field = $video_target['field'];
        $target_instance = $video_target['instance'];
        // $target_entity_wrapper = $target_video_entity_wrappers[$child_delta];
        $target_entity_type = $target_entity_wrapper->type();
        $target_entity = $target_entity_wrapper->value();
        if(!$target_entity) continue;

        $target_items = field_get_items($target_entity_type, $target_entity, $video_target['field_name']);
        if($target_items){
          module_load_include('inc', 'video_embed_field', 'video_embed_field.field');
          if($settings['video_style']){
            if($use_colorbox){
              $element[$delta][$child_delta] = video_embed_field_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, array('type' => 'video_embed_field_thumbnail_colorbox') + $display);
            } else {
              $element[$delta][$child_delta] = video_embed_field_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, array('type' => 'video_embed_field') + $display);
            }
          } else {
            $element[$delta][$child_delta] = video_embed_field_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, array('type' => 'video_embed_field_thumbnail') + $display);
          }
        }
      }
    }
  }

  // Randomize
  if($settings['random']){
    $keys = array_keys($element);
    shuffle($keys);
    $random = array();
    foreach ($keys as $key){
      $elements = $element[$key];
      $child_keys = array_keys($elements);
      shuffle($child_keys);
      foreach ($child_keys as $child_key){
        $random[$key][$child_key] = $element[$key][$child_key];
      }
    }
    $element = $random;
  }

  // Limit
  if($settings['limit']){
    foreach($element as $delta => $elements){
      $element[$delta] = array_slice($element[$delta], 0, $settings['limit'], true);
    }
    $element = array_slice($element, 0, $settings['limit'], true);
  }

  // Link to content
  if($use_colorbox){
    foreach($element as $delta => &$items){
      foreach($items as $item_delta => &$el){
        foreach($el as $id => &$e){
          if (module_exists('file_force') && $settings['force_download']) {
            $e['#prefix'] = '<div class="field-item">';
            $e['#suffix'] = l('Download', file_create_url($e['#item']['uri']), array('attributes' => array('class' => array('download')), 'query' => array('download' => 1))) . '</div>';
          }
        }
      }
    }
  } else {
    foreach($element as $delta => &$items){
      foreach($items as $item_delta => &$el){
        foreach($el as $id => &$e){
          if($settings['as_url']){
            $e = array(
              '#markup' => !empty($e['#image_style']) ? image_style_url($e['#image_style'], $e['#item']['uri']) : file_create_url($e['#item']['uri'])
            );
          }
          elseif ($settings['image_link'] == 'content') {
            $e['#path'] = entity_uri($entity_type, $entity);
          }
          elseif ($settings['image_link'] == 'file') {
            $e['#path']['path'] = file_create_url($e['#item']['uri']);

            if (module_exists('file_force') && $settings['force_download']) {
              $e['#path']['options']['query']['download'] = 1;
            }
          }
          // Nested link render
          elseif (isset($target_link_entity_wrapper)) {
            if(isset($target_link_entity_wrapper[$delta])){
              $link_target_entity_wrapper = $target_link_entity_wrapper[$delta];
              $link_target_entity_type = $link_target_entity_wrapper->type();
              $link_target_entity = $link_target_entity_wrapper->value();
              $link_target_items = field_get_items($link_target_entity_type, $link_target_entity, $link_target['field_name']);
              if(isset($link_target_items[$id]['url'])){
                $e['#path'] = [];

                $e['#path']['path'] = url($link_target_items[$id]['url']);
                $e['#path']['options'] = $link_target_items[$id]['attributes'];
              }
            }
          }
        }
      }
    }
  }

  return $element;
}

// /**
//  * Implements hook_field_formatter_view().
//  */
// function _asset_widget_asset_image_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
//   $settings = $display['settings'];
//   $element = array();
//   $use_colorbox = !empty($settings['colorbox']) && module_exists('colorbox');
//   $target_image_entity_wrappers = array();
//   $target_video_entity_wrappers = array();

//   if(($settings['field_name'] && $image_fields = asset_entityreference_contains($field, 'image')) && isset($image_fields[$settings['field_name']])){
//     $image_target = $image_fields[$settings['field_name']];
//     $target_image_entity_wrappers = asset_entityreference_get($entity_type, $entity, $image_target);
//   }

//   if(($settings['field_video_name'] && $video_fields = asset_entityreference_contains($field, 'video_embed_field')) && isset($video_fields[$settings['field_video_name']])){
//     $video_target = $video_fields[$settings['field_video_name']];
//     $target_video_entity_wrappers = asset_entityreference_get($entity_type, $entity, $video_target);
//   }

//   if(!empty($target_image_entity_wrappers) || !empty($target_video_entity_wrappers)){

//     // Randomize
//     if($settings['random']){
//       $keys = array_keys($items);
//       shuffle($keys);
//       $random = array();
//       foreach ($keys as $key){
//         $random[$key] = $items[$key];
//       }
//       $items = $random;
//     }

//     // Limit
//     if($settings['limit']){
//       $items = array_slice($items, 0, $settings['limit'], true);
//     }

//     // Nested link field support
//     if (!$use_colorbox && !in_array($settings['image_link'], array('content','file'))) {
//       $link_fields = asset_field_contains($field, $instance, 'link_field');
//       if(isset($link_fields[$settings['image_link']])){
//         $link_target = $link_fields[$settings['image_link']];
//         $target_link_entity_wrapper = asset_entityreference_get($entity_type, $entity, $link_target);
//       }
//     }

//     foreach ($items as $delta => $item) {

//       // Images
//       if(isset($target_image_entity_wrappers[$delta])){
//         $target_field = $image_target['field'];
//         $target_instance = $image_target['instance'];
//         $target_entity_wrapper = $target_image_entity_wrappers[$delta];
//         $target_entity_type = $target_entity_wrapper->type();
//         $target_entity = $target_entity_wrapper->value();
//         if(!$target_entity) continue;

//         $target_items = field_get_items($target_entity_type, $target_entity, $image_target['field_name']);
//         if($target_items){
//           module_load_include('inc', 'image', 'image.field');

//           if($use_colorbox){
//             $element[$delta] = colorbox_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display);
//           }
//           else{
//             $element[$delta] = image_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display);
//           }

//           if($settings['as_bg']){
//             foreach($element[$delta] as &$el){
//               $el['#original_theme'] = $el['#theme'];
//               $el['#theme'] = 'asset_image_bg';
//               $el['#attributes']['class'][] = $settings['as_bg_class'];
//             }
//           }
//         }
//       }

//       // Videos
//       if(isset($target_video_entity_wrappers[$delta])){
//         $target_field = $video_target['field'];
//         $target_instance = $video_target['instance'];
//         $target_entity_wrapper = $target_video_entity_wrappers[$delta];
//         $target_entity_type = $target_entity_wrapper->type();
//         $target_entity = $target_entity_wrapper->value();
//         if(!$target_entity) continue;

//         $target_items = field_get_items($target_entity_type, $target_entity, $video_target['field_name']);
//         if($target_items){
//           module_load_include('inc', 'video_embed_field', 'video_embed_field.field');
//           $element[$delta] = video_embed_field_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, array('type' => 'video_embed_field_thumbnail') + $display);

//         }
//       }

//       if(!$use_colorbox){
//         if($settings['as_url']){
//           foreach($element[$delta] as &$e){
//             $element[$delta] = array(
//               '#markup' => !empty($e['#image_style']) ? image_style_url($e['#image_style'], $e['#item']['uri']) : file_create_url($e['#item']['uri'])
//             );
//           }
//         }
//         if ($settings['image_link'] == 'content') {
//           foreach($element[$delta] as &$e){
//             if(isset($e['#path']['path'])){
//               $e['#path'] = entity_uri($entity_type, $entity);
//             }
//           }
//         }
//         if ($settings['image_link'] == 'file') {
//           foreach($element[$delta] as &$e){
//             $e['#path']['path'] = file_create_url($e['#item']['uri']);
//           }
//         }
//         // Nested link render
//         elseif (isset($target_link_entity_wrapper)) {
//           if(isset($target_link_entity_wrapper[$delta])){
//             $link_target_entity_wrapper = $target_link_entity_wrapper[$delta];
//             $link_target_entity_type = $link_target_entity_wrapper->type();
//             $link_target_entity = $link_target_entity_wrapper->value();
//             $link_target_items = field_get_items($link_target_entity_type, $link_target_entity, $link_target['field_name']);
//             if(isset($link_target_items[0]['url'])){
//               foreach($element[$delta] as $i => &$e){
//                 $e['#path']['path'] = url($link_target_items[$i]['url']);
//                 $e['#path']['options'] = $link_target_items[$i]['attributes'];
//               }
//             }
//           }
//         }
//       }
//     }

//   }

//   return $element;
// }

/**
 * Render a nested image field.
 */
// function _asset_widget_asset_image_view_image($entity_type, $entity, $delta, $target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display){
//   $settings = $display['settings'];
//   module_load_include('inc', 'image', 'image.field');
//   $element[$delta] = image_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display);
//   if($settings['as_url']){
//     foreach($element[$delta] as &$e){
//       $element[$delta] = array(
//         '#markup' => !empty($e['#image_style']) ? image_style_url($e['#image_style'], $e['#item']['uri']) : file_create_url($e['#item']['uri'])
//       );
//     }
//   }
//   if ($settings['image_link'] == 'content') {
//     foreach($element[$delta] as &$e){
//       if(isset($e['#path']['path'])){
//         $e['#path'] = entity_uri($entity_type, $entity);
//       }
//     }
//   }
//   // Nested link render
//   elseif (isset($target_link_entity_wrapper)) {
//     if(isset($target_link_entity_wrapper[$delta])){
//       $link_target_entity_wrapper = $target_link_entity_wrapper[$delta];
//       $link_target_entity_type = $link_target_entity_wrapper->type();
//       $link_target_entity = $link_target_entity_wrapper->value();
//       $link_target_items = field_get_items($link_target_entity_type, $link_target_entity, $link_target['field_name']);
//       if(isset($link_target_items[0]['url'])){
//         foreach($element[$delta] as $i => &$e){
//           $e['#path']['path'] = url($link_target_items[$i]['url']);
//           $e['#path']['options'] = $link_target_items[$i]['attributes'];
//         }
//       }
//     }
//   }
//   return $element;
// }
