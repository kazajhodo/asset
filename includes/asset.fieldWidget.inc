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
    'asset_text' => array(
      'label' => t('ASSET | Text'),
      'field types' => array('text', 'text_long', 'text_with_summary'),
      'settings' => array(
        'asset_remove'  => 0,
        'trim_length'   => 0,
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
      'settings' => array('field_name' => '', 'image_style' => '', 'image_link' => '', 'as_url' => FALSE, 'first_only' => FALSE, 'random' => FALSE),
    ),
    'asset_gallery' => array(
      'label' => t('ASSET | Gallery'),
      'field types' => array('entityreference'),
      'settings' => array('field_name' => '', 'image_style' => '', 'image_thumb_style' => ''),
    ),
    'asset_file' => array(
      'label' => t('ASSET | File'),
      'field types' => array('file'),
    ),
    'asset_file_icon' => array(
      'label' => t('ASSET | File Icon'),
      'field types' => array('file'),
      'settings' => array('show_download' => TRUE, 'show_title' => TRUE),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function asset_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  form_load_include($form_state, 'inc', 'asset', 'widgets/' . $display['type']);
  $function = '_asset_widget_' . $display['type'] . '_settings_form';
  if(function_exists($function)){
    return $function($field, $instance, $view_mode, $form, $form_state);
  }
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function asset_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  module_load_include('inc', 'asset', 'widgets/' . $display['type']);
  $function = '_asset_widget_' . $display['type'] . '_settings_summary';
  if(function_exists($function)){
    return $function($field, $instance, $view_mode);
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function asset_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  module_load_include('inc', 'asset', 'widgets/' . $display['type']);
  $function = '_asset_widget_' . $display['type'] . '_view';
  if(function_exists($function)){
    return $function($entity_type, $entity, $field, $instance, $langcode, $items, $display);
  }
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
