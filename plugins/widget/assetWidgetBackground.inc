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
      'image' => '',
      'position' => 'center center',
      'repeat' => 'no-repeat',
      'size' => 'cover',
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
    // $form['#attributes']['class'][] = 'field-inline';

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

    $image = '';
    $trigger = isset($_POST['_triggering_element_value']) ? $_POST['_triggering_element_value'] : '';
    if(!in_array($trigger, array('Remove','Upload')) && isset($values['image']) && $file = file_load($values['image'])){
      $image = theme('image_style', array('style_name' => 'thumbnail', 'path' => $file->uri, 'attributes' => array('style' => 'height:50px;width:auto;float:left;padding-right:20px')));
    }
    $form['image'] = array(
      '#title' => t('Image'),
      '#type' => 'managed_file',
      '#description' => t('Allowed extensions: gif png jpg jpeg.'),
      '#upload_location' => 'public://asset/image/',
      '#upload_validators' => array(
        'file_validate_extensions' => array('gif png jpg jpeg'),
      ),
      '#default_value' => isset($values['image']) ? $values['image'] : '',
      '#field_prefix' => $image,
    );
    $form['position'] = array(
      '#title' => t('Position'),
      '#description' => t('The position of the background image.'),
      '#type' => 'select',
      '#options' => array('left top','left center','left bottom','right top','right center','right bottom','center top','center center','center bottom'),
      '#default_value' => isset($values['position']) ? $values['position'] : '',
    );

    $form['repeat'] = array(
      '#title' => t('Repeat'),
      '#description' => t('The repeat of the background image.'),
      '#type' => 'select',
      '#options' => array('no-repeat','repeat','repeat-x','repeat-y'),
      '#default_value' => isset($values['repeat']) ? $values['repeat'] : '',
    );

    $form['size'] = array(
      '#title' => t('Size'),
      '#description' => t('The size of the background image.'),
      '#type' => 'select',
      '#options' => array('auto','cover','contain'),
      '#default_value' => isset($values['size']) ? $values['size'] : '',
    );

    return $form;
  }

  public function instanceFormSubmit(&$values){
    $current = $this->getValues();
    if($values['image'] != $current['image']){
      // Remove current image.
      if($file = file_load($current['image'])){
        file_delete($file, TRUE);
      }
      // Set saved image as permanent.
      if(!empty($values['image'])){
        $file = file_load($values['image']);
        $file->status = FILE_STATUS_PERMANENT;
        file_usage_add($file, 'asset', 'asset_instance', 1);
        file_save($file);
      }
    }
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

    if(!empty($values['image'])){
      $file = file_load($values['image']);
      $vars['wrapper_styles']['background-image'] = 'background-image:url('.file_create_url($file->uri).');';

      if(!empty($values['size'])){
        $vars['wrapper_styles']['background-size'] = 'background-size:'.$values['size'].';';
      }

      if(!empty($values['repeat'])){
        $vars['wrapper_styles']['background-repeat'] = 'background-repeat:'.$values['repeat'].';';
      }

      if(!empty($values['position'])){
        $vars['wrapper_styles']['background-position'] = 'background-position:'.$values['position'].';';
      }
    }
  }

}
