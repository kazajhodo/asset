<?php


/**
 * Preprocess asset entities. Add instance widget formatters to output.
 */
function _asset_preprocess_asset(&$vars) {
  $asset = $vars['asset'];
  $instance = $asset->instance;
  $asset_type = asset_type_load($asset->type);
  $view_mode = $vars['view_mode'];
  $key = 'asset-' . $asset->id . '-' . $asset->instance->id;
  $vars['classes_array'] = array();
  $vars['classes_array'][] = 'asset';
  $vars['classes_array'][] = drupal_html_class($asset->type);
  $vars['classes_array'][] = drupal_html_class(($view_mode == 'asset' ? 'embed' : $view_mode));
  $vars['classes_array'][] = $key;
  $vars['element_type'] = 'div';

  // Get view path if allowed.
  $vars['path'] = '';
  if($asset_type->data['viewable'] && $asset_type->data['pathauto']){
    $vars['path'] = url(asset_url($asset->id));
  }

  // Add default teaser override option.
  array_splice($vars['theme_hook_suggestions'], 1, 0, array('asset__' . $vars['view_mode']));

  if(!empty($asset_type->data['render_title'])){
    $display = $vars['view_mode'] == 'asset' ? 'embed' : $vars['view_mode'];
    $extra = field_info_extra_fields('asset', $asset->type, 'display');
    if(isset($extra['title']) && !empty($extra['title']['display'][$display]['visible'])){
      $vars['content']['title'] = array(
        '#type' => 'html_tag',
        '#tag' => 'h3',
        '#attributes' => array(
          'class' => 'title',
        ),
        '#value' => $asset->title,
        '#weight' => $extra['title']['display'][$display]['weight'],
      );
    }
  }

  // If view mode is asset, we need to process it through the enabled widgets.
  if(in_array($vars['view_mode'], array('embed'))){
    foreach($instance->widgets as $widget_id => $widget){
      if(!empty($asset->asset_preview)){
        $widget->instanceRenderPreview($vars);
      }
      else{
        $widget->instanceRender($vars);
      }
    }
  }
  // If view mode is NOT asset, we still need to run it through global widgets.
  else{
    foreach($instance->widgets as $widget_id => $widget){
      if(!$widget->getGlobal()) continue;
      if(!empty($asset->asset_preview)){
        $widget->instanceRenderPreview($vars);
      }
      else{
        $widget->instanceRender($vars);
      }
    }
  }
  $vars['prefix'] = isset($vars['prefix']) ? implode($vars['prefix'],'') : '';
  $vars['suffix'] = isset($vars['suffix']) ? implode($vars['suffix'],'') : '';

  if(isset($vars['wrapper_styles'])){
    $style = ".$key{";
    $style .= implode($vars['wrapper_styles'],'');
    $style .= '}';
    $vars['content']['asset_bg']['#markup'] = '<style media="screen">'.$style.'</style>';
  }

  if(isset($vars['content_prefix'])){
    $vars['content']['#prefix'] = isset($vars['content']['#prefix']) ? $vars['content']['#prefix'] : '';
    $vars['content']['#prefix'] = implode($vars['content_prefix'],'') . $vars['content']['#prefix'];
  }

  if(isset($vars['content_suffix'])){
    array_reverse($vars['content_suffix']);
    $vars['content']['#suffix'] = isset($vars['content']['#suffix']) ? $vars['content']['#suffix'] : '';
    $vars['content']['#suffix'] = $vars['content']['#suffix'] . implode($vars['content_suffix'],'');
  }

  // Preview mode which uses instanceRenderPreview().
  if(!empty($asset->asset_preview)){
    // Add data attributes for use elsewhere.
    $vars['attributes_array']['data-aid'] = $asset->id;
    $vars['attributes_array']['data-type'] = $asset->type;
    // Instance id should always be 0 and never null.
    $instance_id = $instance->id ? $instance->id : 0;
    $vars['attributes_array']['data-iid'] = $instance_id = $instance->id ? $instance->id : 0;
  }

  // Full mode view.
  if($vars['view_mode'] === 'full'){
    $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/css/asset.full.scss';
    if(!empty($asset_type->data['usage_info']) && entity_access('update', 'asset', $asset)){
      $usage = asset_usage($asset->id);
      $vars['content']['usage'] = array(
        '#theme' => 'asset_usage',
        '#usage' => $usage,
        '#weight' => 100,
      );
    }
    if(!empty($asset_type->data['instance_info']) && $instances = asset_load_all_instances($asset->id)){
      $vars['content']['instances'] = array(
        '#type' => 'fieldset',
        '#title' => t('Available Instances'),
        '#weight' => 200,
      );
      $vars['content']['instances']['#prefix'] = '<div class="asset-instances">';
      $vars['content']['instances']['#suffix'] = '</div>';
      foreach($instances as $instance_id => $asset_with_instance){
        $vars['content']['instances'][$instance_id]['#prefix'] = '<div class="asset-instance">';
        $vars['content']['instances'][$instance_id]['#suffix'] = '</div>';
        $vars['content']['instances'][$instance_id]['info'] = array();
        $vars['content']['instances'][$instance_id]['info']['#prefix'] = '<div class="asset-instance-info">';
        $vars['content']['instances'][$instance_id]['info']['#suffix'] = '</div>';
        $links = menu_contextual_links('asset', 'admin/content/assets/'.$asset->id.'/instance', array($instance_id));
        if(!empty($links)){
          foreach($links as &$link){
            $title = &$link['title'];
            fett_icon_link($title, $link, TRUE, TRUE);
          }
          $vars['content']['instances'][$instance_id]['info']['ops']['#markup'] = theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline-list', 'asset-instance-ops', 'right'))));
        }
        $label = '<a name="'.$instance_id.'"></a>Instance #'.$instance_id;
        if($instance_id == $asset->instance_id) $label .= ' <span class="asset-instance-default">Default</span>';
        $vars['content']['instances'][$instance_id]['info']['title']['#markup'] = '<div class="asset-instance-title">'.$label.'</div>';
        if(!empty($usage['content'][$instance_id])){
          $vars['content']['instances'][$instance_id]['info']['usage'] = array(
            '#theme' => 'asset_usage_instance',
            '#usage' => $usage['content'][$instance_id],
            '#weight' => 100,
          );
        }
        $vars['content']['instances'][$instance_id]['entity'] = asset_render($asset_with_instance, 'asset');
        $vars['content']['instances'][$instance_id]['entity']['#prefix'] = '<div class="asset-instance-render">';
        $vars['content']['instances'][$instance_id]['entity']['#suffix'] = '</div>';
      }
    }
  }

  // Send element type to children.
  foreach (element_children($vars['content']) as $field_name) {
    $element = &$vars['content'][$field_name];
    $element['#element_type'] = $vars['element_type'];
  }
}

function theme_asset_usage($vars){
  $usage = $vars['usage'];
  $output = '<div class="asset-usage">';

  // Replace with #attached
  drupal_add_css(drupal_get_path('module', 'asset') . '/css/asset.usage.css');

  $output .= '<div class="row asset-usage-totals">';
  $output .= '<div class="medium-4 columns">';
  $output .= '<dl>';
  $output .= '<dt>'.t('Total usage').'</dt>';
  $total = isset($usage['total']) ? $usage['total'] : 0;
  $output .= '<dd><span class="label">' . $total . '</span></dd>';
  $output .= '</dl>';
  $output .= '</div>';
  $output .= '<div class="medium-4 columns">';
  $output .= '<dl>';
  $output .= '<dt>'.t('Content usage').'</dt>';
  $total = isset($usage['total_content']) ? $usage['total_content'] : 0;
  $output .= '<dd><span class="label secondary">' . $total . '</span></dd>';
  $output .= '</dl>';
  $output .= '</div>';
  $output .= '<div class="medium-4 columns">';
  $output .= '<dl>';
  $output .= '<dt>'.t('Reference usage').'</dt>';
  $total = isset($usage['total_referenced']) ? $usage['total_referenced'] : 0;
  $output .= '<dd><span class="label secondary">' . $total . '</span></dd>';
  $output .= '</dl>';
  $output .= '</div>';
  $output .= '</div>';

  if(!empty($usage['total_content'])){
    $items = array();
    foreach($usage['content'] as $uses){
      foreach($uses as $use){
        $items[] = theme('asset_usage_link', array('use' => $use));
      }
    }
    $output .= '<dl class="asset-usage-content asset-usage-list">';
    $output .= '<dt>'.t('Content usage').'</dt>';
    $output .= '<dd>'.theme('item_list', array('items' => $items,'attributes'=>array('class'=>array('small-block-grid-2','medium-block-grid-3','large-block-grid-4')))).'</dd>';
    $output .= '</dl>';
  }

  if(!empty($usage['total_referenced'])){
    $items = array();
    foreach($usage['referenced'] as $use){
      $items[] = theme('asset_usage_link', array('use' => $use));
    }
    $output .= '<dl class="asset-usage-referenced asset-usage-list">';
    $output .= '<dt>'.t('Reference usage').'</dt>';
    $output .= '<dd>'.theme('item_list', array('items' => $items,'attributes'=>array('class'=>array('small-block-grid-2','medium-block-grid-3','large-block-grid-4')))).'</dd>';
    $output .= '</dl>';
  }

  $output .= '</div>';

  drupal_add_library('system', 'drupal.collapse');
  return theme('fieldset', array('element' => array('#title' => t('Asset Usage') . ' ('.$total.')', '#children' => $output, '#collapsible' => TRUE, '#collapsed' => TRUE, '#attributes' => array('class' => array('asset-usage', 'collapsible', 'collapsed')))));
}

function theme_asset_usage_instance($vars){
  $usage = $vars['usage'];
  $output = '';
  if(!empty($usage)){
    $items = array();
    foreach($usage as $use){
      $items[] = theme('asset_usage_link', array('use' => $use));
    }
    $output .= '<dl class="asset-usage-list">';
    $output .= '<dt>'.t('Instance usage').'</dt>';
    $output .= '<dd>'.theme('item_list', array('items' => $items,'attributes'=>array('class'=>array('small-block-grid-2','medium-block-grid-3','large-block-grid-4')))).'</dd>';
    $output .= '</dl>';
  }
  return theme('fieldset', array('element' => array('#title' => '', '#children' => $output, '#attributes' => array('class' => array('asset-usage', 'asset-usage-instance')))));
  return $output;
}

function theme_asset_usage_link($vars){
  $use = $vars['use'];
  $output = '';
  if($entity = entity_load_single($use['entity_type'], $use['entity_id'])){
    $entity_info = entity_get_info($use['entity_type']);
    list($entity_id, , $bundle_name) = entity_extract_ids($use['entity_type'], $entity);
    $bundle = isset($entity_info['bundles'][$bundle_name]) ? $entity_info['bundles'][$bundle_name] : NULL;
    if(!empty($entity_info['label'])) $output .= '<span class="label">' . $entity_info['label'] . '</span>';
    if(!empty($bundle['label'])) $output .= '<span class="label secondary">' . $bundle['label'] . '</span>';
    if(!empty($output)) $output .= ' ';
    $output .= entity_label($use['entity_type'], $entity);
    if($url = entity_uri($use['entity_type'], $entity)){
      $output = l($output, $url['path'], array('html'=>true));
    }
  }
  return $output;
}

/**
 * Displays the list of available asset types for asset creation.
 *
 * @ingroup themeable
 */
function theme_asset_add_list($vars) {
  $content = $vars['content'];
  return theme('node_add_list', array('content' => $content));
}

/**
 * Implements theme_field().
 */
function theme_asset_field($vars) {
  $output = '';
  if ( !empty($vars['element_type']) ) {
    if (!$vars['label_hidden']) {
      $output .= '<' . $vars['element_type'] . ' ' . $vars['title_attributes'] . '>' . $vars['label'] . (!empty($vars['no_colon']) ? '' : ':') . '&nbsp;</' . $vars['element_type'] . '>';
    }
    if(count($vars['items']) > 1){
      $output .= '<' . $vars['element_type'] . ' class="field-items"' . $vars['content_attributes'] . '>';
    }
  }
  foreach ($vars['items'] as $item) {
    $output .= drupal_render($item);
  }
  if ( !empty($vars['element_type']) ) {
    if(count($vars['items']) > 1){
      $output .= '</' . $vars['element_type'] . '>';
    }
    $output = '<' . $vars['element_type'] . ' class="' . $vars['classes'] . '"' . $vars['attributes'] . '>' . $output . '</' . $vars['element_type'] . '>';
  }
  return $output;
}

/**
 * Returns HTML for a link to a file.
 *
 * @param $vars
 *   An associative array containing:
 *   - file: A file object to which the link will be created.
 *   - icon_directory: (optional) A path to a directory of icons to be used for
 *     files. Defaults to the value of the "file_icon_directory" variable.
 *
 * @ingroup themeable
 */
function theme_asset_file_link($vars) {
  $file = $vars['file'];
  $icon_directory = drupal_get_path('module', 'asset') . '/images/icons';
  $ext = pathinfo($file->uri, PATHINFO_EXTENSION);
  $label = '';
  if($vars['show_download']){
    $label = '<span class="file-label file-tag">' . file_create_label($ext) . '</span><span class="file-download file-tag">'.t('Download').'</span>';
  }

  $url = file_create_url($file->uri);
  $icon = theme('asset_file_icon', array('file' => $file, 'icon_directory' => $icon_directory));

  // Add CSS
  drupal_add_css(drupal_get_path('module','asset') . '/css/asset.widget.file.icon.scss', array('group' => CSS_SYSTEM));

  // Set options as per anchor format described at
  // http://microformats.org/wiki/file-format-examples
  $options = array(
    'html' => TRUE,
    'attributes' => array(
      'type' => $file->filemime . '; length=' . $file->filesize,
    ),
  );

  $link_text = '';
  if($vars['show_title']){
    // Use the description as the link text if available.
    if (empty($file->description)) {
      $link_text = $file->filename;
    }
    else {
      $link_text = $file->description;
      $options['attributes']['title'] = check_plain($file->filename);
    }
    $link_text = '<span class="file-text">'.$link_text.'</span>';
  }

  return '<span class="asset icon file ' . $ext . '">' . l($label . $icon . $link_text, $url, $options) . '</span>';
}

/**
 * Returns HTML for an image with an appropriate icon for the given file.
 *
 * @param $vars
 *   An associative array containing:
 *   - file: A file object for which to make an icon.
 *   - icon_directory: (optional) A path to a directory of icons to be used for
 *     files. Defaults to the value of the "file_icon_directory" variable.
 *
 * @ingroup themeable
 */
function theme_asset_file_icon($vars) {
  $file = $vars['file'];
  $icon_directory = $vars['icon_directory'];

  $mime = check_plain($file->filemime);
  $icon_url = file_icon_url($file, $icon_directory);
  return '<img class="file-icon" alt="" title="' . $mime . '" src="' . $icon_url . '" />';
}

/**
 * Get a human-readable label for file objects.
 */
function file_create_label($ext){
  $label = 'File';
  switch($ext){
    case 'doc':
    case 'docx':
      $label = 'Doc';
      break;
    case 'xls':
    case 'xlsx':
      $label = 'Excel';
      break;
    case 'zip':
      $label = 'Zip';
      break;
    case 'ppt':
    case 'pptx':
      $label = 'Powerpoint';
      break;
    case 'pdf':
      $label = 'PDF';
      break;
    case 'jpg':
    case 'jpeg':
    case 'png':
    case 'gif':
      $label = 'Image';
      break;
  }
  return $label;
}



/**
 * Returns HTML for an Colorbox image field formatter.
 *
 * @param $vars
 *   An associative array containing:
 *   - item: An array of image data.
 *   - image_style: An optional image style.
 *   - path: An array containing the link 'path' and link 'options'.
 *
 * @ingroup themeable
 */
function theme_asset_image_bg($vars) {
  $image = theme($vars['original_theme'], $vars);

  // Replace <img> with <div>.
  $vars['attributes']['class'][] = 'asset-image-bg';
  $regex = '#<img.*src="([^"]*)"([^>]*)>#';
  $replace = '<div'.drupal_attributes($vars['attributes']).' style="background-image: url($1);"></div>';
  $image = preg_replace($regex, $replace, $image);

  return $image;
}
