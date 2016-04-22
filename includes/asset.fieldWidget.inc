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
  $info = array(
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
      'settings' => array(
        'field_name' => '',
        'field_video_name' => '',
        'image_style' => '',
        'video_style' => '',
        'image_link' => '',
        'as_bg' => false,
        'as_bg_class' => '',
        'as_url' => false,
        'limit' => '',
        'random' => false,
        'colorbox_video_only' => '',
      ),
    ),
    'asset_gallery' => array(
      'label' => t('ASSET | Gallery'),
      'field types' => array('entityreference'),
      'settings' => array('field_name' => '', 'field_video_name' => '', 'image_style' => '', 'image_thumb_style' => '', 'video_style' => ''),
    ),
    'asset_file' => array(
      'label' => t('ASSET | File'),
      'field types' => array('file'),
      'settings' => array('title' => ''),
    ),
    'asset_file_icon' => array(
      'label' => t('ASSET | File Icon'),
      'field types' => array('file'),
      'settings' => array('show_download' => true, 'show_title' => true),
    ),
    'asset_file_svg' => array(
      'label' => t('ASSET | SVG'),
      'field types' => array('file'),
      'settings' => array(
        'wrapper_tag' => 'figure',
        'classes' => null,
      ),
    ),
    'asset_link' => array(
      'label' => t('ASSET | Link'),
      'field types' => array('entityreference'),
      'settings' => array('icon' => 'file', 'label' => 'Download', 'field_name' => ''),
    ),
  );
  if(module_exists('colorbox')){
    $settings = colorbox_field_formatter_info();
    $info['asset_image']['settings']['colorbox'] = false;
    $info['asset_image']['settings'] += $settings['colorbox']['settings'];
  }
  return $info;
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
 * Implements hook_field_group_format_settings_alter().
 */
function asset_field_group_format_settings($group) {
  $field_group_types = field_group_formatter_info();
  $mode = $group->mode == 'form' ? 'form' : 'display';
  $formatter = $field_group_types[$mode][$group->format_type];
  $form = array();
  if ($group->entity_type == 'asset' && $mode == 'form') {
    $form['instance_settings']['ief_hide'] = array(
      '#type' => 'checkbox',
      '#title' => t('Do not show when adding/editing within an Inline Entity Form.'),
      '#default_value' => isset($group->format_settings['instance_settings']['ief_hide']) ? $group->format_settings['instance_settings']['ief_hide'] : (isset($formatter['instance_settings']['ief_hide']) ? $formatter['instance_settings']['ief_hide'] : ''),
      '#weight' => 200,
    );
  }
  return $form;
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
