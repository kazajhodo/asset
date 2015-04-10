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
