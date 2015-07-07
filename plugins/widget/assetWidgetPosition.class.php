<?php

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
      '#type' => 'radios',
      '#title' => t('Position'),
      '#options' => array(
        '' => t('Default'),
        'left' => '<i class="fa fa-align-left"></i>',
        'center' => '<i class="fa fa-align-center"></i>',
        'right' => '<i class="fa fa-align-right"></i>',
      ),
      '#attributes' => array('class' => array('explode')),
      '#default_value' => isset($values['position']) ? $values['position'] : 'center',
    );

    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetPosition.css';
    if(!empty($values['position'])){
      $vars['classes_array'][] = 'asset-position-' . $values['position'];
    }
    else{
      $vars['element_type'] = 'span';
      $vars['classes_array'][] = 'asset-position-default';
    }
  }

}
