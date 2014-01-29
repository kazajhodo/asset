<?php

/**
 * @file
 * Asset editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */

/**
 * UI controller.
 */
class AssetUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {

    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    $items[$this->path] = array(
      'title' => 'Assets',
      'description' => 'Add edit and update assets.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('access administration pages'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );

    // Change the overview menu type for the list of assets.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;

    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add a asset',
      'description' => 'Add a new asset',
      'page callback'  => 'asset_add_page',
      'access callback'  => 'asset_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
      'file' => 'asset.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])

    );

    // Add menu items to add each different type of entity.
    foreach (asset_get_types() as $type) {
      $items[$this->path . '/add/' . $type->type] = array(
        'title' => 'Add ' . $type->label,
        'page callback' => 'asset_form_wrapper',
        'page arguments' => array($type->type),
        'access callback' => 'asset_access',
        'access arguments' => array('edit', $type),
        'file' => 'asset.admin.inc',
        'file path' => drupal_get_path('module', $this->entityInfo['module'])
      );
    }

    // Loading and editing asset entities
    $items[$this->path . '/asset/' . $wildcard] = array(
      'page callback' => 'asset_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'asset_access',
      'access arguments' => array('edit', $id_count + 1),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'asset.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    $items[$this->path . '/asset/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );

    $items[$this->path . '/asset/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'asset_delete_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'asset_access',
      'access arguments' => array('edit', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'asset.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );

    // Menu item for viewing assets
    $items['asset/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'asset_page_title',
      'title arguments' => array(1),
      'page callback' => 'asset_page_view',
      'page arguments' => array(1),
      'access callback' => 'asset_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );
    return $items;
  }


  /**
   * Create the markup for the add Asset Entities page within the class
   * so it can easily be extended/overriden.
   */
  public function addPage() {
    $item = menu_get_item();
    $content = system_admin_menu_block($item);

    if (count($content) == 1) {
      $item = array_shift($content);
      drupal_goto($item['href']);
    }

    return theme('asset_add_list', array('content' => $content));
  }

}


/**
 * Form callback wrapper: create or edit a asset.
 *
 * @param $asset
 *   The asset object being edited by this form.
 *
 * @see asset_edit_form()
 */
function asset_form_wrapper($asset) {
  if(!is_a($asset, 'Asset')){
    $asset = asset_create(array('type' => $asset));
  }
  // Add the breadcrumb for the form's location.
  asset_set_breadcrumb();
  return drupal_get_form('asset_edit_form', $asset);
}


/**
 * Form callback wrapper: delete a asset.
 *
 * @param $asset
 *   The asset object being edited by this form.
 *
 * @see asset_edit_form()
 */
function asset_delete_form_wrapper($asset) {
  // Add the breadcrumb for the form's location.
  //asset_set_breadcrumb();
  return drupal_get_form('asset_delete_form', $asset);
}


/**
 * Form callback: create or edit a asset.
 *
 * @param $asset
 *   The asset object to edit or for a create form an empty asset object
 *     with only a asset type defined.
 */
function asset_edit_form($form, &$form_state, $asset) {
  $asset_type = asset_type_load($asset->type);

  // Add the default field elements.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t($asset_type->label . ' Name'),
    '#default_value' => isset($asset->name) ? $asset->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  $form['data']['#tree'] = TRUE;
  // $form['data']['sample_data'] = array(
  //   '#type' => 'checkbox',
  //   '#title' => t('An interesting asset switch'),
  //   '#default_value' => isset($asset->data['sample_data']) ? $asset->data['sample_data'] : 1,
  // );

  // Add the field related form elements.
  $form_state['asset'] = $asset;
  field_attach_form('asset', $asset, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save asset'),
    '#submit' => $submit + array('asset_edit_form_submit'),
  );

  if (!empty($asset->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete asset'),
      '#suffix' => l(t('Cancel'), 'admin/content/assets'),
      '#submit' => $submit + array('asset_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'asset_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the asset form
 */
function asset_edit_form_validate(&$form, &$form_state) {
  $asset = $form_state['asset'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('asset', $asset, $form, $form_state);
}


/**
 * Form API submit callback for the asset form.
 *
 * @todo remove hard-coded link
 */
function asset_edit_form_submit(&$form, &$form_state) {

  $asset = entity_ui_controller('asset')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the asset and go back to the list of assets

  // Add in created and changed times.
  if ($asset->is_new = isset($asset->is_new) ? $asset->is_new : 0){
    $asset->created = time();
  }

  $asset->changed = time();

  $asset->save();
  $form_state['redirect'] = 'admin/content/assets';
}

/**
 * Form API submit callback for the delete button.
 *
 * @todo Remove hard-coded path
 */
function asset_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/assets/asset/' . $form_state['asset']->id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a asset.
 *
 * @param $asset
 *   The asset to delete
 *
 * @see confirm_form()
 */
function asset_delete_form($form, &$form_state, $asset) {
  $form_state['asset'] = $asset;

  $form['#submit'][] = 'asset_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete asset %name?', array('%name' => $asset->name)),
    'admin/content/assets/asset',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for asset_delete_form
 */
function asset_delete_form_submit($form, &$form_state) {
  $asset = $form_state['asset'];

  asset_delete($asset);

  drupal_set_message(t('The asset %name has been deleted.', array('%name' => $asset->name)));
  watchdog('asset', 'Deleted asset %name.', array('%name' => $asset->name));

  $form_state['redirect'] = 'admin/content/assets';
}



/**
 * Page to add Asset Entities.
 *
 * @todo Pass this through a proper theme function
 */
function asset_add_page() {
  $controller = entity_ui_controller('asset');
  return $controller->addPage();
}


/**
 * Displays the list of available asset types for asset creation.
 *
 * @ingroup themeable
 */
function theme_asset_add_list($variables) {
  $content = $variables['content'];
  $output = '';
  if ($content) {
    $output = '<dl class="asset-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    if (user_access('administer asset types')) {
      $output = '<p>' . t('Asset Entities cannot be added because you have not created any asset types yet. Go to the <a href="@create-asset-type">asset type creation page</a> to add a new asset type.', array('@create-asset-type' => url('admin/structure/asset_types/add'))) . '</p>';
    }
    else {
      $output = '<p>' . t('No asset types have been created yet for you to use.') . '</p>';
    }
  }

  return $output;
}

/**
 * Menu callback for settings form.
 */
function asset_settings($form, $form_state) {

  drupal_set_message('Coming soon.');

  return system_settings_form($form);
}





/**
 * Sets the breadcrumb for administrative asset pages.
 */
function asset_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('Assets'), 'admin/content/assets'),
  );

  drupal_set_breadcrumb($breadcrumb);
}



