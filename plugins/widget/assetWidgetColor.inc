<?php

$plugin = array(
  'title' => t('Text Color'),
  'description' => t('Provides the ability to select dark or light text color.'),
  'class' => 'assetWidgetColor',
  'public' => TRUE,
  'weight' => 10,
);

class assetWidgetColor extends assetWidget {

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
    $form['#title'] = t('Text Color');
    $form['#collapsible'] = TRUE;
    $form['#collapsed'] = TRUE;

    $form['color'] = array(
      '#type' => 'radios',
      '#options' => array('dark' => t('Dark'), 'light' => t('Light')),
      '#default_value' => !empty($values['color']) ? $values['color'] : 'dark',
    );
    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['color'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetColor.css';
      $vars['classes_array'][] = 'asset-color-' . $values['color'];
    }
  }

}
