<?php

function asset_view_modes(){
  return array('asset' => t('Asset'),'asset_preview' => t('Asset Preview'),'asset_full' => t('Asset Full'));
}

/**
 * Implements hook_field_formatter_info().
 */
function asset_field_formatter_info() {
  $default_tags = array(
    'a', 'b', 'br', 'em', 'i', 'p', 'strong', 'u', 'ul',
  );

  return array(
    // 'asset_entity_view' => array(
    //   'label' => t('Rendered asset'),
    //   'description' => t('Display the referenced assets rendered by entity_view().'),
    //   'field types' => array('entityreference'),
    //   'settings' => array(
    //     'view_mode' => 'asset',
    //   ),
    // ),
    'asset_text_trimmed' => array(
      'label' => t('ASSET | Trimmed'),
      'field types' => array('text', 'text_long', 'text_with_summary'),
      'settings' => array(
        'asset_remove'  => 1,
        'trim_length'   => 600,
        'ellipsis'      => 1,
        'word_boundary' => 1,
        'token_replace' => 0,
        'filter'        => 'input',
        'format'        => 'plain_text',
        'allowed_html'  => $default_tags,
        'autop'         => 0,
        'use_summary'   => 0,
      ),
    ),
    'asset_image' => array(
      'label' => t('ASSET | Image'),
      'field types' => array('entityreference'),
      'settings' => array('field_name' => '', 'image_style' => '', 'image_link' => ''),
    ),
    'asset_file' => array(
      'label' => t('ASSET | File'),
      'field types' => array('file'),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function asset_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();

  switch($display['type']){
    // case 'asset_entity_view':
    //   $entity_info = entity_get_info($field['settings']['target_type']);
    //   $options = asset_view_modes();
    //   if (!empty($entity_info['view modes'])) {
    //     foreach ($entity_info['view modes'] as $view_mode => $view_mode_settings) {
    //       $options[$view_mode] = $view_mode_settings['label'];
    //     }
    //   }
    //   $element['view_mode'] = array(
    //     '#type' => 'select',
    //     '#options' => $options,
    //     '#title' => t('View mode'),
    //     '#default_value' => $settings['view_mode'],
    //     '#access' => count($options) > 1,
    //   );
    //   break;
    case 'asset_image':
      if(isset($field['settings']['handler_settings']['target_bundles']) && count($field['settings']['handler_settings']['target_bundles']) === 1){
        $target_type = $field['settings']['target_type'];
        $target_bundle = key($field['settings']['handler_settings']['target_bundles']);
        $field_names = array();
        foreach (field_info_instances($target_type, $target_bundle) as $target_field_name => $target_instance) {
          $target_field = field_info_field($target_field_name);
          if ($target_field['type'] == 'image') {
            $field_names[$target_field['field_name']] = $target_instance['label'] . ' [' . $target_field['field_name'] . ']';
          }
        }
        if(!empty($field_names)){
          module_load_include('inc', 'image', 'image.field');
          $element = image_field_formatter_settings_form($field, $instance, $view_mode, $form, $form_state);
          $element['field_name'] = array(
            '#title' => t('Target image field'),
            '#type' => 'select',
            '#default_value' => $settings['field_name'],
            '#required' => TRUE,
            '#options' => $field_names,
            '#weight' => -100,
          );
        }
      }
      break;
    case 'asset_text_trimmed':

      $elid_trim   = drupal_html_id('asset_trim');
      $elid_filter = drupal_html_id('asset_filter');

      $element['trim_length'] = array(
        '#id'               => $elid_trim,
        '#type'             => 'textfield',
        '#title'            => t('Trim length'),
        '#description'      => t("Set this to 0 if you don't want to cut the text. Otherwise, input a positive integer."),
        '#size'             => 10,
        '#default_value'    => $settings['trim_length'],
        '#element_validate' => array('element_validate_integer'),
        '#required'         => TRUE,
      );
      $element['asset_remove'] = array(
        '#type' => 'checkbox',
        '#title' => t('Remove assets from output'),
        '#default_value' => $settings['asset_remove'] ? 1 : 0,
      );

      $element['ellipsis'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('Ellipsis'),
        '#description'   => t('If checked, a "..." will be added if a field was trimmed.'),
        '#default_value' => $settings['ellipsis'],
        '#states'        => array(
          'visible' => array(
            '#' . $elid_trim  => array('!value' => '0'),
          ),
        ),
      );

      $element['word_boundary'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('Word Boundary'),
        '#description'   => t('If checked, this field will be trimmed only on a word boundary.'),
        '#default_value' => $settings['word_boundary'],
        '#states'        => array(
          'visible' => array(
            '#' . $elid_trim  => array('!value' => '0'),
          ),
        ),
      );

      $element['use_summary'] = array(
        '#type'           => 'checkbox',
        '#title'          => t('Use Summary'),
        '#description'    => t('If a summary exists, use it.'),
        '#default_value'  => $settings['use_summary'],
      );

      $token_link = _asset_browse_tokens($instance['entity_type']);

      $element['token_replace'] = array(
        '#type'          => 'checkbox',
        '#description'   => t('Replace text pattern. e.g %node-title-token or %node-author-name-token, by token values.', array(
                              '%node-title-token'       => '[node:title]',
                              '%node-author-name-token' => '[node:author:name]',
                            )) . ' ' . $token_link,
        '#title'         => t('Token Replace'),
        '#default_value' => $settings['token_replace'],
      );

      $element['filter'] = array(
        '#id'      => $elid_filter,
        '#title'   => t('Filter'),
        '#type'    => 'select',
        '#options' => array(
          'none'   => t('None'),
          'input'      => t('Selected Text Format'),
          'php'        => t('Limit allowed HTML tags'),
          'drupal'     => t('Drupal'),
        ),
        '#default_value' => $settings['filter'],
      );

      $element['format'] = array(
        '#title'         => t('Format'),
        '#type'          => 'select',
        '#options'       => array(),
        '#default_value' => $settings['format'],
        '#states'        => array(
          'visible' => array(
            '#' . $elid_filter  => array('value' => 'drupal'),
          ),
        ),
      );

      $formats = filter_formats();

      foreach ($formats as $format => $info) {
        $element['format']['#options'][$format] = $info->name;
      }

      if (empty($settings['allowed_html'])) {
        $tags = '';
      }
      else {
        $tags = '<' . implode('> <', $settings['allowed_html']) . '>';
      }

      $element['allowed_html'] = array(
        '#type'              => 'textfield',
        '#title'             => t('Allowed HTML tags'),
        '#description'       => t('See <a href="@link" target="_blank">filter_xss()</a> for more information', array(
                                  '@link' => 'http://api.drupal.org/api/drupal/includes%21common.inc/function/filter_xss/7',
                                )),
        '#default_value'     => $tags,
        '#element_validate'  => array('_asset_validate_allowed_html'),
        '#states'            => array(
          'visible' => array(
            '#' . $elid_filter  => array('value' => 'php'),
          ),
        ),
      );

      $element['autop'] = array(
        '#title'         => t('Converts line breaks into HTML (i.e. &lt;br&gt; and &lt;p&gt;) tags.'),
        '#type'          => 'checkbox',
        '#return_value'  => 1,
        '#default_value' => $settings['autop'],
        '#states'        => array(
          'invisible' => array(
            '#' . $elid_filter  => array('!value' => 'php'),
          ),
        ),
      );

      $element['br'] = array('#markup' => '<br/>');

      break;
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function asset_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = array();

  if ($display['type'] == 'asset_image') {
    module_load_include('inc', 'image', 'image.field');
    $summary = t('Target field: @style', array('@style' => $settings['field_name'] ? $settings['field_name'] : 'NONE'));
    $summary .= '<br />' . image_field_formatter_settings_summary($field, $instance, $view_mode);
    return $summary;
  }


  // if ($display['type'] == 'asset_entity_view') {
  //   $entity_info = entity_get_info($field['settings']['target_type']);
  //   $asset_view_modes = asset_view_modes();
  //   $view_mode_label = isset($asset_view_modes[$settings['view_mode']]) ? $asset_view_modes[$settings['view_mode']] : $settings['view_mode'];
  //   if (isset($entity_info['view modes'][$settings['view_mode']]['label'])) {
  //     $view_mode_label = $entity_info['view modes'][$settings['view_mode']]['label'];
  //   }
  //   $summary[] = t('Rendered as @mode', array('@mode' => $view_mode_label));
  // }

  if ($display['type'] == 'asset_text_trimmed') {
    $yes = '<i class="fa fa-check text-success"></i>';
    $no  = '<i class="fa fa-times text-warning"></i>';

    $summary[] = t('Strip Assets') . ': ' . ((isset($settings['asset_remove']) && $settings['asset_remove']) ? $yes : $no);

    if ($settings['trim_length'] > 0) {
      $summary[] = t('Trim length') . ': ' . $settings['trim_length'];
      $summary[] = t('Ellipsis') . ': ' . ((isset($settings['ellipsis']) && $settings['ellipsis']) ? $yes : $no);
      $summary[] = t('Word Boundary') . ': ' . ((isset($settings['word_boundary']) && $settings['word_boundary']) ? $yes : $no);
      $summary[] = t('Use Summary') . ': ' . ((isset($settings['use_summary']) && $settings['use_summary']) ? $yes : $no);
    }

    $token_link = _asset_browse_tokens($instance['entity_type']);
    $summary[] = t('Token Replace') . ': ' . ((isset($settings['token_replace']) && $settings['token_replace']) ? ($yes . '. ' . $token_link) : $no);

    switch ($settings['filter']) {
      case 'drupal':
        $formats = filter_formats();
        $format  = isset($formats[$settings['format']]) ? $formats[$settings['format']]->name : t('Unknown');
        $summary[] = t('Filter: @filter', array('@filter' => t('Drupal')));
        $summary[] = t('Format: @format', array('@format' => $format));
        break;

      case 'php':
        $text = array();
        if (empty($settings['allowed_html'])) {
          $text[] = t('Remove all HTML tags.');
        }
        else {
          $tags = '<' . implode('> <', $settings['allowed_html']) . '>';
          $text[] = t('Limit allowed HTML tags: !tags.', array('!tags' => $tags));
        }
        if (!empty($settings['autop'])) {
          $text[] = t('Convert line breaks into HTML.');
        }
        $summary[] = t('Filter: @filter', array('@filter' => implode(' ', $text)));
        break;

      case 'input':
        $summary[] = t('Filter: @filter', array('@filter' => t('Selected Text Format')));
        break;

      default:
        $summary[] = t('Filter: @filter', array('@filter' => t('None')));
        break;
    }
  }

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_prepare_view().
 */
function asset_field_formatter_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items, $displays) {
  if($field['type'] !== 'entityreference') return;

  $target_ids = array();

  // Collect every possible entity attached to any of the entities.
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      if (is_array($item) && isset($item['target_id'])) {
        $target_ids[] = $item['target_id'];
      }
    }
  }

  if ($target_ids) {
    $target_entities = entity_load($field['settings']['target_type'], $target_ids);
  }
  else {
    $target_entities = array();
  }

  // Iterate through the fieldable entities again to attach the loaded data.
  foreach ($entities as $id => $entity) {
    $rekey = FALSE;

    foreach ($items[$id] as $delta => $item) {
      // Check whether the referenced entity could be loaded.
      if (is_array($item) && isset($item['target_id']) && isset($target_entities[$item['target_id']])) {
        // Replace the instance value with the term data.
        $items[$id][$delta]['entity'] = $target_entities[$item['target_id']];
        $items[$id][$delta]['entity_type'] = $field['settings']['target_type'];
        // Check whether the user has access to the referenced entity.
        $has_view_access = (entity_access('view', $field['settings']['target_type'], $target_entities[$item['target_id']]) !== FALSE);
        $has_update_access = (entity_access('update', $field['settings']['target_type'], $target_entities[$item['target_id']]) !== FALSE);
        $items[$id][$delta]['access'] = ($has_view_access || $has_update_access);
      }
      // We allow the full object to be passed through.
      elseif (is_object($item)) {
        list($entity_id, , $bundle) = entity_extract_ids($field['settings']['target_type'], $item);
        // Do nothing.
        $items[$id][$delta] = array();
        $items[$id][$delta]['target_id'] = $entity_id;
        $items[$id][$delta]['entity'] = $item;
        $items[$id][$delta]['entity_type'] = $field['settings']['target_type'];
        // Check whether the user has access to the referenced entity.
        $has_view_access = (entity_access('view', $field['settings']['target_type'], $item) !== FALSE);
        $has_update_access = (entity_access('update', $field['settings']['target_type'], $item) !== FALSE);
        $items[$id][$delta]['access'] = ($has_view_access || $has_update_access);
      }
      // Otherwise, unset the instance value, since the entity does not exist.
      else {
        unset($items[$id][$delta]);
        $rekey = TRUE;
      }
    }

    if ($rekey) {
      // Rekey the items array.
      $items[$id] = array_values($items[$id]);
    }
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function asset_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];

  switch ($display['type']) {
    case 'asset_image':
      if($settings['field_name']){
        foreach ($items as $delta => $item) {
          if($item['entity_type'] == 'asset'){
            $target_entity_type = $item['entity_type'];
            $target_entity = $item['entity'];
            list(, , $target_bundle) = entity_extract_ids($target_entity_type, $target_entity);
            $target_field = field_info_field($settings['field_name']);
            $target_instance = field_info_instance($target_entity_type, $settings['field_name'], $target_bundle);
            $target_items = field_get_items($target_entity_type, $target_entity, $settings['field_name']);
            module_load_include('inc', 'image', 'image.field');
            $element[$delta] = image_field_formatter_view($target_entity_type, $target_entity, $target_field, $target_instance, $langcode, $target_items, $display);
          }
        }
      }
      break;
    // case 'asset_entity_view':

    //   // Rebuild the items list to contain only those with access.
    //   foreach ($items as $key => $item) {
    //     if (empty($item['access'])) {
    //       unset($items[$key]);
    //     }
    //   }
    //   foreach ($items as $delta => $item) {
    //     // Protect ourselves from recursive rendering.
    //     static $depth = 0;
    //     $depth++;
    //     if ($depth > 20) {
    //       throw new EntityReferenceRecursiveRenderingException(t('Recursive rendering detected when rendering entity @entity_type(@entity_id). Aborting rendering.', array('@entity_type' => $entity_type, '@entity_id' => $item['target_id'])));
    //     }

    //     $entity = clone $item['entity'];
    //     unset($entity->content);
    //     $element[$delta] = entity_view($field['settings']['target_type'], array($item['target_id'] => $entity), $settings['view_mode'], $langcode, FALSE);

    //     $depth = 0;
    //   }
    //   break;
    case 'asset_text_trimmed':
      $settings   = $display['settings'];
      $token_data = array($entity_type => $entity);

      foreach ($items as $delta => $item) {

        if ($settings['use_summary'] && !empty($item['summary'])) {
          $output = $item['summary'];
        }
        else {
          $output = $item['value'];
        }

        if ($settings['asset_remove']) {
          $output = asset_token_remove($output);
        }

        if ($settings['token_replace']) {
          $output = token_replace($output, $token_data);
        }

        switch ($settings['filter']) {
          case 'drupal':
            $output = check_markup($output, $settings['format'], $langcode);
            break;

          case 'php':
            $output = filter_xss($output, $settings['allowed_html']);
            if ($settings['autop']) {
              $output = _filter_autop($output);
            }
            break;

          case 'input':
            $output = check_markup($output, $item['format'], $langcode);
            break;
        }

        if ($settings['trim_length'] > 0) {
          $options  = array(
            'word_boundary' => $settings['word_boundary'],
            'max_length'    => $settings['trim_length'],
            'ellipsis'      => $settings['ellipsis'],
          );
          $output = asset_trim_text($output, $options);
        }

        $element[$delta] = array('#markup' => $output);
      }
      break;

    case 'asset_file':

      foreach ($items as $delta => $item) {
        if($entity_type == 'asset'){
          $item['description'] = $entity->name;
        }
        $element[$delta] = array(
          '#theme' => 'file_link',
          '#file' => (object) $item,
        );
      }
      break;
  }

  return $element;
}

/**
 * Trim text.
 *
 * @param string $text
 *   The string is being trimmed.
 *
 * @param array $options
 *   An associative array containing:
 *   - html: TRUE means that text is in HTML.
 *   - max_length: The maximum number of characters the a field can be.
 *   - word_boundary: If checked, this field be trimmed only on a word boundary.
 *   - ellipsis: If TRUE, a "..." will be added if a field was trimmed.
 *
 * @return string
 *   The trimmed string.
 */
function asset_trim_text($text, $options) {
  if (!isset($options['html'])) {
    $options['html'] = TRUE;
  }

  if (drupal_strlen($text) > $options['max_length']) {
    $text = drupal_substr($text, 0, $options['max_length']);

    // TODO: replace this with cleanstring of ctools.
    if (!empty($options['word_boundary'])) {
      $regex = "(.*)\b.+";

      if (function_exists('mb_ereg')) {
        mb_regex_encoding('UTF-8');
        $found = mb_ereg($regex, $text, $matches);
      }
      else {
        $found = preg_match("/$regex/us", $text, $matches);
      }

      if ($found) {
        $text = $matches[1];
      }
    }

    // Remove scraps of HTML entities from the end of a strings.
    $text = rtrim(preg_replace('/(?:<(?!.+>)|&(?!.+;)).*$/us', '', $text));

    if (!empty($options['ellipsis'])) {
      $text .= t('...');
    }
  }

  if (!empty($options['html'])) {
    $text = _filter_htmlcorrector($text);
  }

  return $text;
}

/**
 * Get a link to browse for available tokens.
 *
 * @param mixed $token_types
 *   A string or array contains the token types. See theme_token_tree() for more
 *   details.
 *
 * @return string
 *   A HTML link
 */
function _asset_browse_tokens($token_types) {
  if (!module_exists('token')) {
    return;
  }

  if (!is_array($token_types)) {
    $token_types = array($token_types);
  }

  $vars['token_types'] = $token_types;

  return theme('token_tree_link', $vars);
}

/**
 * Add available tokens link to text_format element.
 *
 * @param array $element
 *   Form element
 *
 * @return array
 *   Form element
 */
function _asset_field_add_token_tree($element) {
  $element['format']['show_token'] = array(
    '#type' => 'container',
    'link'  => array(
      '#markup' => _asset_browse_tokens($element['#entity_type']),
    ),
    '#attributes' => array(
      'class' => array('clearfix'),
    ),
    '#weight' => 99,
  );

  return $element;
}

/**
 * Change tags from string to array to speed up the formatter view.
 *
 * @param array $element
 *   Form element.
 *
 * @param array $form_state
 *   Form state.
 */
function _asset_validate_allowed_html($element, &$form_state) {
  $tags  = array();
  $value = isset($element['#value']) ? trim($element['#value']) : '';

  if (!empty($value)) {
    $tags = preg_split('/\s+|<|>/', $value, -1, PREG_SPLIT_NO_EMPTY);
  }

  drupal_array_set_nested_value($form_state['values'], $element['#parents'], $tags);
}
