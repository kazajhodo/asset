<?php

$plugin = array(
  'title' => t('Placeholder'),
  'description' => t('Provides the ability to use a placeholder when in preview mode.'),
  'class' => 'assetWidgetPlaceholder',
  'public' => FALSE,
  'global' => TRUE,
  'weight' => 10,
);

class assetWidgetPlaceholder extends assetWidget {

  public function instanceRenderPreview(&$vars) {
    $asset_type = asset_type_load($vars['asset']->type);
    $vars['content'] = array();
    $vars['content']['#attached']['css'][] = drupal_get_path('module', 'asset') . '/plugins/widget/css/assetWidgetPlaceholder.css';
    $vars['content']['placeholder'] = array(
      '#markup' => '<div class="asset-placeholder"><div class="fa fa-'.$asset_type->data['icon'].'"><div class="asset-placeholder-hide">&nbsp;</div></div>&nbsp;&nbsp;<strong>'.$asset_type->name.':</strong> '.$vars['asset']->name.'</div>',
    );
    // We always want to fall back to default tpl file when previewing.
    $vars['theme_hook_suggestions'] = array('asset');
  }
}
