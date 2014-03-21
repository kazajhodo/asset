<?php

class AssetInstanceHandler_Link extends AssetInstanceHandler_Abstract {

  public function instance_form(&$form, &$form_state, $settings, $defaults) {
    $form['link'] = array(
      '#id' => 'instance-asset-link',
      '#type' => 'textfield',
      '#title' => t('Link URL'),
      '#size' => 60,
      '#maxlength' => 128,
      '#weight' => 2,
      '#default_value' => !empty($defaults['link']) ? $defaults['link'] : '',
    );

    if(module_exists('linkit')){

      $plugins = linkit_profile_field_load_all();
      if(!empty($plugins)){
        // Load the profile.
        $profile = array_pop($plugins);
        // Load the insert plugin for the profile.
        $insert_plugin = linkit_insert_plugin_load($profile->data['insert_plugin']['plugin']);

        $element = &$form['link'];

        // Set the field ID.
        $field_id = $element['#id'];

        $field_js = array(
          'data' => array(
            'linkit' => array(
              'fields' => array(
                $field_id => array(
                  'profile' => $profile->name,
                  'insert_plugin' => $profile->data['insert_plugin']['plugin'],
                  'url_method' => $profile->data['insert_plugin']['url_method'],
                  // @TODO: Add autocomplete settings.
                ),
              ),
            ),
          ),
          'type' => 'setting',
        );

        // Link fields can have a title field.
        if ($element['#type'] == 'link_field') {
          if (isset($instance['settings']['title']) && in_array($instance['settings']['title'], array('optional', 'required'))) {
            $field_js['data']['linkit']['fields'][$field_id]['title_field'] = $element['#id'] . '-title';
          }
        }

        // Attach js files and settings Linkit needs.
        $element['#attached']['library'][] = array('linkit', 'base');
        $element['#attached']['library'][] = array('linkit', 'field');
        $element['#attached']['js'][] = $insert_plugin['javascript'];
        $element['#attached']['js'][] = $field_js;

        $button_text = !empty($instance['settings']['linkit']['button_text']) ? $instance['settings']['linkit']['button_text'] : t('Search');
        $element['#suffix'] = '<a style="margin-top:-14px; margin-bottom:20px;" class="btn btn-info btn-sm btn-block linkit-field-button linkit-field-' . $field_id . '" href="#"><i class="fa fa-search"></i> ' . $button_text . ' Local Content</a>';
      }
    }

    $form['new'] = array(
      '#type' => 'checkbox',
      '#title' => t('Open link in a new window'),
      '#id' => 'exo-link-new',
      '#weight' => 3,
      '#default_value' => !empty($defaults['new']) ? $defaults['new'] : '',
      '#states' => array(
        'visible' => array(
          ':input[name="data[link]"]' => array('filled' => TRUE),
        ),
      ),
    );
  }

  public function instance_render(&$vars, $settings, $config) {
    if(!empty($settings['link'])){
      $attributes = array();
      $attributes['href'] = url($settings['link']);
      if(!empty($settings['new'])){
        $attributes['target'] = '_blank';
      }
      $vars['content']['link_prefix'] = array(
        '#markup' => '<a '.drupal_attributes($attributes).'>',
        '#weight' => -100
      );
      $vars['content']['link_suffix'] = array(
        '#markup' => '</a>',
        '#weight' => 100
      );
    }
    // if(empty($settings['link'])) return;
    // foreach (element_children($element['asset']) as $key) {
    //   foreach (element_children($element['asset'][$key]) as $delta) {
    //     $field = &$element['asset'][$key][$delta];
    //     $prefix = '<a href="' . $settings['link'] . '">';
    //     $suffix = '</a>';
    //     $field['#prefix'] = $prefix . (!empty($field['#prefix']) ? $field['#prefix'] : '');
    //     $field['#suffix'] = (!empty($field['#suffix']) ? $field['#suffix'] : '') . $suffix;
    //   }
    // }
  }

  public function instance_render_preview(&$vars, $settings, $config) {
    // Don't do anything.
  }

}
