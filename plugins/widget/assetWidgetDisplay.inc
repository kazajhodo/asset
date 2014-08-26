<?php

$plugin = array(
  'title' => t('Display'),
  'description' => t('Provides the ability to change the display settings of an asset.'),
  'class' => 'assetWidgetDisplay',
  'public' => TRUE,
  'weight' => -1,
);

class assetWidgetDisplay extends assetWidget {

  public function instanceDefaults(){
    return array(
      'full' => 0,
      'reset' => 0,
      'indent_top' => 0,
      'indent_bottom' => 0,
      'indent_right' => 0,
      'indent_left' => 0,
      'flush_top' => 0,
      'flush_bottom' => 0,
    );
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $form['#type'] = 'fieldset';
    $form['#title'] = t('Display');
    $form['#collapsible'] = TRUE;
    $form['#collapsed'] = TRUE;
    $form['#attached']['css']['asset-widget-display'] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetDisplay.css';

    $form['full'] = array(
      '#type' => 'checkbox',
      '#title' => t('Full Width'),
      '#description' => t('Content wrapper will expand to fill the entire screen from left to right.'),
      '#default_value' => isset($values['full']) ? $values['full'] : 0,
    );

    $form['reset'] = array(
      '#type' => 'checkbox',
      '#title' => t('Restore content to grid'),
      '#description' => t('Content will be contrained to the grid. Useful when content wrapper is set to full width.'),
      '#default_value' => isset($values['reset']) ? $values['reset'] : 0,
    );

    $form['indent_top'] = array(
      '#type' => 'checkbox',
      '#title' => '<i class="fa fa-angle-up"></i> ' . t('Top padding'),
      '#description' => t('Content will be indented from the top edge of the content wrapper.'),
      '#default_value' => isset($values['indent_top']) ? $values['indent_top'] : 0,
    );

    $form['indent_bottom'] = array(
      '#type' => 'checkbox',
      '#title' => '<i class="fa fa-angle-down"></i> ' . t('Bottom padding'),
      '#description' => t('Content will be indented from the bottom edge of the content wrapper.'),
      '#default_value' => isset($values['indent_bottom']) ? $values['indent_bottom'] : 0,
    );

    $form['indent_right'] = array(
      '#type' => 'checkbox',
      '#title' => '<i class="fa fa-angle-right"></i> ' . t('Right padding'),
      '#description' => t('Content will be indented from the right edge of the content wrapper.'),
      '#default_value' => isset($values['indent_right']) ? $values['indent_right'] : 0,
    );

    $form['indent_left'] = array(
      '#type' => 'checkbox',
      '#title' => '<i class="fa fa-angle-left"></i> ' . t('Left padding'),
      '#description' => t('Content will be indented from the left edge of the content wrapper.'),
      '#default_value' => isset($values['indent_left']) ? $values['indent_left'] : 0,
    );

    $form['flush_top'] = array(
      '#type' => 'checkbox',
      '#title' => '<i class="fa fa-caret-up"></i> ' . t('Flush Top'),
      '#description' => t('Content wrapper will have no margin on the top.'),
      '#default_value' => isset($values['flush_top']) ? $values['flush_top'] : 0,
    );

    $form['flush_bottom'] = array(
      '#type' => 'checkbox',
      '#title' => '<i class="fa fa-caret-down"></i> ' . t('Flush Bottom'),
      '#description' => t('Content wrapper will have no margin on the bottom.'),
      '#default_value' => isset($values['flush_bottom']) ? $values['flush_bottom'] : 0,
    );

    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['full'])){
      $vars['content']['#attached']['js'][''] = drupal_get_path('module', 'asset') . '/plugins/widget/js/assetWidgetDisplay.js';
      $vars['classes_array'][] = 'asset-full';
    }
    if(!empty($values['reset'])){
      $vars['prefix'][] = '<div class="asset-reset">';
      $vars['suffix'][] = '</div>';
    }
    if(!empty($values['indent_top'])){
      $vars['content']['#attached']['css']['asset-widget-display'] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetDisplay.css';
      $vars['classes_array'][] = 'asset-indent-t';
    }
    if(!empty($values['indent_right'])){
      $vars['content']['#attached']['css']['asset-widget-display'] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetDisplay.css';
      $vars['classes_array'][] = 'asset-indent-r';
    }
    if(!empty($values['indent_bottom'])){
      $vars['content']['#attached']['css']['asset-widget-display'] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetDisplay.css';
      $vars['classes_array'][] = 'asset-indent-b';
    }
    if(!empty($values['indent_left'])){
      $vars['content']['#attached']['css']['asset-widget-display'] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetDisplay.css';
      $vars['classes_array'][] = 'asset-indent-l';
    }
    if(!empty($values['flush_top'])){
      $vars['content']['#attached']['css']['asset-widget-display'] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetDisplay.css';
      $vars['classes_array'][] = 'asset-flush-t';
    }
    if(!empty($values['flush_bottom'])){
      $vars['content']['#attached']['css']['asset-widget-display'] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetDisplay.css';
      $vars['classes_array'][] = 'asset-flush-b';
    }
  }

}
