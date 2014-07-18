<?php

$plugin = array(
  'title' => t('Position'),
  'description' => t('Provides the ability to position an asset (left, right, center).'),
  'class' => 'assetWidgetPosition',
  'public' => TRUE,
  'weight' => 0,
);

class assetWidgetPosition extends assetWidget {

  public function instanceDefaults(){
    return array(
      'position' => 'center'
    );
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $form['position'] = array(
      '#type' => 'select',
      '#title' => t('Position'),
      '#options' => array(
        '' => t('- None -'),
        'center' => t('Center'),
        'left' => t('Left'),
        'right' => t('Right'),
      ),
      '#default_value' => isset($values['position']) ? $values['position'] : 'center',
    );

    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['position'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetPosition.css';
      $vars['classes_array'][] = 'asset-position-' . $values['position'];
    }
  }

}
