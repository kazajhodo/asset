<?php

class AssetInstanceHandler_Placeholder extends AssetInstanceHandler_Abstract {

  public function instance_render_preview(&$vars, $settings, $config) {
    $asset_type = asset_type_load($vars['asset']->type);
    $vars['content'] = array();
    $vars['content']['placeholder'] = array(
      '#markup' => '<div class="exo-asset-placeholder"><div class="fa fa-'.$asset_type->data['icon'].'"><div class="exo-hide">&nbsp;</div></div>&nbsp;&nbsp;<strong>'.$asset_type->label.':</strong> '.$vars['asset']->name.'</div>',
    );
  }

}
