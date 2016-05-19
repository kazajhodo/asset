<?php

/**
 * Implements hook_field_formatter_settings_form().
 */
function _asset_widget_asset_text_settings_form($field, $instance, $view_mode, $form, &$form_state){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();

  $elid_trim   = drupal_html_id('asset_trim');
  $elid_filter = drupal_html_id('asset_filter');

  $element['asset_remove'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove assets from output'),
    '#default_value' => $settings['asset_remove'] ? 1 : 0,
  );

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
      'drupal'     => t('Other Text Format'),
      'php'        => t('Limit allowed HTML tags'),
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

  $element['br'] = array('#markup' => '<br>');

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function _asset_widget_asset_text_settings_summary($field, $instance, $view_mode){
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();

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
      $summary[] = t('Filter: @filter', array('@filter' => t('Text Format')));
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

  return implode('<br>', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function _asset_widget_asset_text_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $element = array();
  $settings = $display['settings'];

  foreach ($items as $delta => $item) {

    if ($settings['use_summary'] && !empty($item['summary'])) {
      $output = check_markup($item['summary'], $settings['format'], $langcode);
    }
    else {
      $output = $item['value'];
    }

    if ($settings['asset_remove']) {
      $output = asset_token_remove($output);
    }

    if ($settings['token_replace']) {
      $output = token_replace($output, array($entity_type => $entity));
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
