<?php

/**
 * @file
 * Contains the 'asset' argument validator plugin.
 */

/**
 * Validate whether an argument is an acceptable asset.
 */
class asset_plugin_argument_validate extends views_plugin_argument_validate {
  function option_definition() {
    $options = parent::option_definition();
    $options['types'] = array('default' => array());
    $options['access'] = array('default' => FALSE, 'bool' => TRUE);
    $options['access_op'] = array('default' => 'view');
    $options['id_type'] = array('default' => 'id');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    $types = asset_get_types();
    $options = array();
    foreach ($types as $type => $info) {
      $options[$type] = check_plain(t($info->name));
    }

    $form['types'] = array(
      '#type' => 'checkboxes',
      '#name' => t('Asset types'),
      '#options' => $options,
      '#default_value' => $this->options['types'],
      '#description' => t('Choose one or more content types to validate with.'),
    );

    $form['access'] = array(
      '#type' => 'checkbox',
      '#name' => t('Validate user has access to the asset'),
      '#default_value' => $this->options['access'],
    );
    $form['access_op'] = array(
      '#type' => 'radios',
      '#name' => t('Access operation to check'),
      '#options' => array('view' => t('View'), 'update' => t('Edit'), 'delete' => t('Delete')),
      '#default_value' => $this->options['access_op'],
      '#dependency' => array('edit-options-validate-options-asset-access' => array(TRUE)),
    );

    $form['id_type'] = array(
      '#type' => 'select',
      '#name' => t('Filter value format'),
      '#options' => array(
        'id' => t('Asset ID'),
        'ids' => t('Asset IDs separated by , or +'),
      ),
      '#default_value' => $this->options['id_type'],
    );
  }

  function options_submit(&$form, &$form_state, &$options = array()) {
    // filter trash out of the options so we don't store giant unnecessary arrays
    $options['types'] = array_filter($options['types']);
  }

  function convert_options(&$options) {
    if (!isset($options['types']) && !empty($this->argument->options['validate_argument_asset_type'])) {
      $options['types'] = isset($this->argument->options['validate_argument_asset_type']) ? $this->argument->options['validate_argument_asset_type'] : array();
      $options['access'] = !empty($this->argument->options['validate_argument_asset_access']);
      $options['access_op'] = isset($this->argument->options['validate_argument_asset_access_op']) ? $this->argument->options['validate_argument_asset_access_op'] : 'view';
      $options['id_type'] = isset($this->argument->options['validate_argument_id_type']) ? $this->argument->options['validate_argument_id_type'] : array();
    }
  }

  function validate_argument($argument) {
    $types = $this->options['types'];

    switch ($this->options['id_type']) {
      case 'id':
        if (!is_numeric($argument)) {
          return FALSE;
        }
        $asset = asset_load($argument);
        if (!$asset) {
          return FALSE;
        }

        if (!empty($this->options['access'])) {
          if (!asset_access($this->options['access_op'], $asset)) {
            return FALSE;
          }
        }

        // Save the name() handlers some work.
        $this->argument->validated_title = check_plain($asset->name);

        if (empty($types)) {
          return TRUE;
        }

        return isset($types[$asset->type]);
      break;
      case 'ids':
        $ids = new stdClass();
        $ids->value = array($argument);
        $ids = views_break_phrase($argument, $ids);
        if ($ids->value == array(-1)) {
          return FALSE;
        }

        $test = drupal_map_assoc($ids->value);
        $names = array();

        $result = db_query("SELECT * FROM {asset} WHERE id IN (:ids)", array(':ids' =>  $ids->value));
        foreach ($result as $asset) {
          if ($types && empty($types[$asset->type])) {
            return FALSE;
          }

          if (!empty($this->options['access'])) {
            if (!asset_access($this->options['access_op'], $asset)) {
              return FALSE;
            }
          }

          $names[] = check_plain($asset->name);
          unset($test[$asset->id]);
        }

        $this->argument->validated_title = implode($ids->operator == 'or' ? ' + ' : ', ', $names);
        // If this is not empty, we did not find a id.
        return empty($test);
    }
  }
}

