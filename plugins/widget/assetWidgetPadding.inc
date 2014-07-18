<?php

$plugin = array(
  'title' => t('Padding'),
  'description' => t('Provides the ability to pad an asset.'),
  'class' => 'assetWidgetPadding',
  'public' => TRUE,
  'weight' => 0,
);

class assetWidgetPadding extends assetWidget {

  public function instanceDefaults(){
    return array(
      'padding' => array(),
    );
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $form['#type'] = 'fieldset';
    $form['#title'] = t('Padding');
    $form['#collapsible'] = TRUE;
    $form['#collapsed'] = TRUE;
    $form['#attributes'] = array('class'=>array('asset-padding-widget'));
    $form['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetPadding.css';

    $form['top'] = array(
      '#type' => 'textfield',
      '#title' => '<i class="fa fa-arrow-up"></i>',
      '#default_value' => isset($values['top']) ? $values['top'] : '',
    );

    $form['left'] = array(
      '#type' => 'textfield',
      '#title' => '<i class="fa fa-arrow-left"></i>',
      '#default_value' => isset($values['left']) ? $values['left'] : '',
    );

    $form['right'] = array(
      '#type' => 'textfield',
      '#title' => '<i class="fa fa-arrow-right"></i>',
      '#default_value' => isset($values['right']) ? $values['right'] : '',
    );

    $form['bottom'] = array(
      '#type' => 'textfield',
      // '#title' => '<i class="fa fa-arrow-down"></i>',
      '#suffix' => '<label class="asset-position-bottom"><i class="fa fa-arrow-down"></i></label>',
      '#default_value' => isset($values['bottom']) ? $values['bottom'] : '',
    );

    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['top']) || !empty($values['left']) || !empty($values['right']) || !empty($values['bottom'])){
      if(!empty($values['top'])) $vars['wrapper_styles']['padding-top'] = 'padding-top:'.$values['top'].'px;';
      if(!empty($values['left'])) $vars['wrapper_styles']['padding-left'] = 'padding-left:'.$values['left'].'px;';
      if(!empty($values['right'])) $vars['wrapper_styles']['padding-right'] = 'padding-right:'.$values['right'].'px;';
      if(!empty($values['bottom'])) $vars['wrapper_styles']['padding-bottom'] = 'padding-bottom:'.$values['bottom'].'px;';
    }
  }

}
