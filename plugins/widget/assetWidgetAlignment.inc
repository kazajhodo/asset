<?php

$plugin = array(
  'title' => t('Alignment'),
  'description' => t('Provides the ability to position an asset in relation to its parent element.'),
  'class' => 'assetWidgetAlignment',
  'public' => TRUE,
  'weight' => 0,
);

class assetWidgetAlignment extends assetWidget {

  public function instanceDefaults(){
    return array(
      'full' => 0,
      'reset' => 0,
      'inset' => 0,
    );
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $form['#type'] = 'fieldset';
    $form['#title'] = t('Alignment');
    $form['#collapsible'] = TRUE;
    $form['#collapsed'] = TRUE;
    $form['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetAlignment.css';

    $form['full'] = array(
      '#type' => 'checkbox',
      '#title' => t('Full width'),
      '#default_value' => isset($values['full']) ? $values['full'] : 0,
    );

    $form['reset'] = array(
      '#type' => 'checkbox',
      '#title' => t('Restore content to grid'),
      '#default_value' => isset($values['reset']) ? $values['reset'] : 0,
    );

    $form['inset'] = array(
      '#type' => 'checkbox',
      '#title' => t('Inset content'),
      '#default_value' => isset($values['inset']) ? $values['inset'] : 0,
    );

    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['full'])){
      $vars['content']['#attached']['js'][''] = drupal_get_path('module', 'asset') . '/plugins/widget/js/assetWidgetAlignment.js';
      $vars['classes_array'][] = 'asset-full';
    }
    if(!empty($values['reset'])){
      $vars['content_prefix'][] = '<div class="asset-reset">';
      $vars['content_suffix'][] = '</div>';
    }
    if(!empty($values['inset'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetAlignment.css';
      $vars['classes_array'][] = 'asset-inset';
    }
  }

}
