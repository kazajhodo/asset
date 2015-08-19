<?php

class assetWidgetImageStyle extends assetWidget {

  public function instanceDefaults(){
    $settings = $this->getFormSettings();
    $defaults = array();
    if(!empty($settings)){
      foreach($settings as $id => $label){
        if(!isset($defaults['image_style'])) $defaults['image_style'] = $id;
      }
    }
    return $defaults;
  }

  public function settingsForm($parents) {
    $defaults = $this->getFormSettings();
    $key = asset_parents_to_name($parents);
    $styles = image_styles();
    $form = array(
      '#type' => 'item',
      '#tree' => TRUE,
      '#title' => t('Enabled styles')
    );
    foreach($styles as $id => $style){
      $form[$id]['#tree'] = TRUE;
      $form[$id]['status'] = array(
        '#type' => 'checkbox',
        '#title' => $style['label'],
        '#default_value' => empty($defaults[$id]) ? 0 : 1,
      );
      $form[$id]['label'] = array(
        '#type' => 'textfield',
        '#title' => t('Label'),
        '#default_value' => empty($defaults[$id]) ? '' : $defaults[$id],
      );
      $form[$id]['label']['#states'] = array(
        'visible' => array(
          ':input[name="'.$key.'[settings]['.$id.'][status]"]' => array('checked' => TRUE),
        ),
      );
    }
    return $form;
  }

  public function settingsFormValidate(&$values) {
    $styles = image_styles();
    if(!empty($values['settings'])){
      foreach($values['settings'] as $style_id => $data){
        if(empty($data['status'])){
          unset($values['settings'][$style_id]);
        }
        else{
          $values['settings'][$style_id] = !empty($data['label']) ? $data['label'] : $styles[$style_id]['label'];
        }
      }
    }
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $options = array('' => 'Default');
    foreach($settings as $id => $label){
      $options[$id] = !empty($label) ? $label : $id;
    }
    $form['image_style'] = array(
      '#type' => 'radios',
      '#title' => t('Image Style'),
      '#options' => $options,
      '#attributes' => array('class' => array('explode')),
      '#default_value' => isset($values['image_style']) ? $values['image_style'] : '',
    );
    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    foreach($vars['content'] as $fieldname => $data){
      if(isset($data['#field_type']) && $data['#field_type'] === 'image'){
        foreach (element_children($vars['content'][$fieldname]) as $delta) {
          $field = &$vars['content'][$fieldname][$delta];
          $field['#theme'] = 'image_formatter';
          $field['#image_style'] = $values['image_style'];
        }
      }
    }
    $vars['classes_array'][] = drupal_html_class('style-' . $values['image_style']);
  }

}
