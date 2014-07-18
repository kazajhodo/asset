<?php

if(module_exists('jquery_colorpicker')){
  $plugin = array(
    'title' => t('Background'),
    'description' => t('Provides the ability to add background colors to an asset.'),
    'class' => 'assetWidgetBackground',
    'public' => FALSE,
    'weight' => 0,
  );
}

class assetWidgetBackground extends assetWidget {

  public function instanceDefaults(){
    return array(
      'primary' => '',
      'secondary' => '',
    );
  }

  public function instanceForm() {
    $form = array();
    $settings = $this->getFormSettings();
    $values = $this->getValues();

    $form['#type'] = 'fieldset';
    $form['#title'] = t('Background');
    $form['#collapsible'] = TRUE;
    $form['#collapsed'] = TRUE;
    $form['#attributes']['class'][] = 'field-inline';

    $form['primary'] = array(
      '#type' => 'jquery_colorpicker',
      '#title' => t('Primary'),
      '#default_value' => isset($values['primary']) ? $values['primary'] : '',
      '#cardinality' => 1,
    );

    $form['secondary'] = array(
      '#type' => 'jquery_colorpicker',
      '#title' => t('Secondary'),
      '#default_value' => isset($values['secondary']) ? $values['secondary'] : '',
      '#cardinality' => 1,
    );

    return $form;
  }

  public function instanceRender(&$vars) {
    $values = $this->getValues();
    if(!empty($values['primary']) || !empty($values['secondary'])){
      $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetBackground.css';
      $primary = '#' . $values['primary'];
      $secondary = '#' . $values['secondary'];
      $vars['classes_array'][] = 'asset-has-background';
      if(!empty($values['primary']) && !empty($values['secondary'])){
        $vars['wrapper_styles']['background'] = "background: $primary;
background: -moz-linear-gradient(top,  $primary 0%, $secondary 100%); /* FF3.6+ */
background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,$primary), color-stop(100%,$secondary)); /* Chrome,Safari4+ */
background: -webkit-linear-gradient(top,  $primary 0%,$secondary 100%); /* Chrome10+,Safari5.1+ */
background: -o-linear-gradient(top,  $primary 0%,$secondary 100%); /* Opera 11.10+ */
background: -ms-linear-gradient(top,  $primary 0%,$secondary 100%); /* IE10+ */
background: linear-gradient(to bottom,  $primary 0%,$secondary 100%); /* W3C */
filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='$primary', endColorstr='$secondary',GradientType=0 ); /* IE6-9 */";
      }
      elseif(!empty($values['primary'])){
        $vars['wrapper_styles']['background'] = 'background:#'.$values['primary'].';';
      }
      elseif(!empty($values['secondary'])){
        $vars['wrapper_styles']['background'] = 'background:#'.$values['secondary'].';';
      }
    }
  }

}
