<?php

class assetWidgetText extends assetWidget {

  public function instanceDefaults(){
    return array(
      'color' => 'dark',
    );
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $form['#type'] = 'fieldset';
    $form['#title'] = t('Text');
    $form['#collapsible'] = TRUE;
    $form['#collapsed'] = TRUE;

    $form['hero'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hero'),
      '#description' => t('Make all text within the content stand out.'),
      '#default_value' => isset($values['hero']) ? $values['hero'] : 0,
    );

    $form['center'] = array(
      '#type' => 'checkbox',
      '#title' => t('Center'),
      '#description' => t('Center all text within the content.'),
      '#default_value' => isset($values['center']) ? $values['center'] : 0,
    );

    $form['color'] = array(
      '#title' => t('Color'),
      '#type' => 'radios',
      '#options' => array('dark' => t('Dark'), 'light' => t('Light')),
      '#default_value' => !empty($values['color']) ? $values['color'] : 'dark',
    );
    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['color'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetText.css';
      $vars['classes_array'][] = 'asset-color-' . $values['color'];
    }
    if(!empty($values['center'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetText.css';
      $vars['classes_array'][] = 'asset-center';
    }
    if(!empty($values['hero'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetText.css';
      $vars['classes_array'][] = 'asset-hero';
    }
  }

}
