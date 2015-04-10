<?php

class assetWidgetGrid extends assetWidget {

  public function instanceDefaults(){
    return array(
      'column' => 1,
      'full' => 0,
      'flush' => 0,
    );
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $form['#type'] = 'fieldset';
    $form['#title'] = t('Grid');
    $form['#collapsible'] = TRUE;
    $form['#collapsed'] = TRUE;

    $columns = array();
    for($i = 1; $i <= 3; $i++) $columns[$i] = $i;

    $form['column'] = array(
      '#type' => 'select',
      '#title' => t('Column consumption'),
      '#options' => $columns,
      '#default_value' => $values['column'],
    );

    $form['full'] = array(
      '#type' => 'checkbox',
      '#title' => t('Span full width'),
      '#default_value' => $values['full'],
    );

    $form['flush'] = array(
      '#type' => 'checkbox',
      '#title' => t('Keep column flush to the edge of the screen'),
      '#default_value' => $values['flush'],
    );
    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['flush'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetGrid.css';
      $vars['classes_array'][] = 'asset-grid-flush';
    }
  }

}
