<?php

class AssetInstanceHandler_Caption extends AssetInstanceHandler_Abstract {

  public function instance_form(&$form, &$form_state, $settings, $defaults) {
    $form['caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption'),
      '#size' => 60,
      '#maxlength' => 256,
      '#weight' => 1,
      '#default_value' => !empty($defaults['caption']) ? $defaults['caption'] : '',
    );
  }

  public function instance_render(&$vars, $settings, $config) {
    if(!empty($settings['caption'])){
      $vars['content']['caption'] = array(
        '#markup' => '<div class="asset-caption">'.$settings['caption'].'</div>',
        '#weight' => 90
      );
    }
  }

}
