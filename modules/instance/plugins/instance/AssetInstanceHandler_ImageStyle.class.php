<?php

class AssetInstanceHandler_ImageStyle extends AssetInstanceHandler_Abstract {

  public function defaults($settings){
    $defaults = array();
    if(!empty($settings['image_styles'])){
      foreach($settings['image_styles'] as $id => $data){
        if(empty($data['enabled'])) continue;
        $defaults['image_style'] = $id;
      }
    }
    return $defaults;
  }

  public function settings_form(&$form, &$form_state, $defaults, $parent_name) {
    $styles = image_styles();
    $form['image_styles'] = array(
      '#type' => 'item',
      '#tree' => TRUE,
      '#title' => t('Enabled styles')
    );
    foreach($styles as $id => $style){
      $form['image_styles'][$id]['#tree'] = TRUE;
      $form['image_styles'][$id]['enabled'] = array(
        '#type' => 'checkbox',
        '#title' => $style['label'],
        '#default_value' => empty($defaults['image_styles'][$id]['enabled']) ? 0 : 1,
      );
      $form['image_styles'][$id]['label'] = array(
        '#type' => 'textfield',
        '#title' => t('Label'),
        '#default_value' => empty($defaults['image_styles'][$id]['label']) ? '' : $defaults['image_styles'][$id]['label'],
      );
      $form['image_styles'][$id]['label']['#states'] = array(
        'visible' => array(
          ':input[name="'.$parent_name.'[image_styles]['.$id.'][enabled]"]' => array('checked' => TRUE),
        ),
      );
    }
  }

  public function instance_form(&$form, &$form_state, $settings, $defaults) {
    $options = array('' => '- None -');
    foreach($settings['image_styles'] as $id => $data){
      if(empty($data['enabled'])) continue;
      $options[$id] = !empty($data['label']) ? t($data['label']) : $id;
    }
    $form['image_style'] = array(
      '#type' => 'select',
      '#title' => t('Image Style'),
      '#options' => $options,
      '#default_value' => isset($defaults['image_style']) ? $defaults['image_style'] : '',
    );
  }

  public function instance_render(&$vars, $settings, $config) {
    foreach($vars['content'] as $fieldname => $data){
      if($data['#field_type'] == 'image'){
        foreach (element_children($vars['content'][$fieldname]) as $delta) {
          $field = &$vars['content'][$fieldname][$delta];
          $field['#theme'] = 'image_formatter';
          $field['#image_style'] = $settings['image_style'];
        }
      }
    }
  }

}
