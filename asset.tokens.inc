<?php

/**
 * @file
 * Token callbacks for the asset module.
 */

/**
 * Implements hook_token_info().
 *
 * @ingroup asset
 */
function asset_token_info() {
  $type = array(
    'name' => t('Assets'),
    'description' => t('Tokens related to individual assets.'),
    'needs-data' => 'asset',
  );

  $asset['id'] = array(
    'name' => t('Asset ID'),
    'description' => t('The unique ID of the asset.'),
  );

  $asset['type'] = array(
    'name' => t('Asset Type'),
    'description' => t('The asset type of the asset.'),
  );

  $asset['title'] = array(
    'name' => t('Asset Title'),
    'description' => t('The asset title/name/label.'),
  );

  $asset['created'] = array(
    'name' => t('Date created'),
    'description' => t('The date the asset was posted.'),
    'type' => 'date',
  );

  $asset['author'] = array(
    'name' => t('Asset Title'),
    'description' => t('The author of the asset.'),
    'type' => 'user',
  );

  return array(
    'types' => array('asset' => $type),
    'tokens' => array('asset' => $asset),
  );
}

/**
 * Implements hook_tokens().
 */
function asset_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  if ($type == 'asset' && !empty($data['asset'])) {
    $asset = $data['asset'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        // Simple key values on the asset.
        case 'id':
          $replacements[$original] = $asset->id;
          break;

        // Simple key values on the asset.
        case 'type':
          $replacements[$original] = $asset->type;
          break;

        case 'title':
          $replacements[$original] = $sanitize ? check_plain($asset->title) : $asset->title;
          break;

          // Default values for the chained tokens handled below.
        case 'author':
          $name = ($asset->uid == 0) ? variable_get('anonymous', t('Anonymous')) : $asset->title;
          $replacements[$original] = $sanitize ? filter_xss($name) : $name;
          break;

        case 'created':
          $replacements[$original] = format_date($asset->created, 'medium', '', NULL, $language_code);
          break;
      }
    }

    if ($author_tokens = token_find_with_prefix($tokens, 'author')) {
      $author = user_load($asset->uid);
      $replacements += token_generate('user', $author_tokens, array('user' => $author), $options);
    }

    if ($created_tokens = token_find_with_prefix($tokens, 'created')) {
      $replacements += token_generate('date', $created_tokens, array('date' => $asset->created), $options);
    }
  }

  return $replacements;
}
