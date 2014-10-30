<?php
/**
 * @file
 * Administration page callbacks for the splash_offer module.
 *
 * @ingroup splash_offer
 * @{
 */

/**
 * Form builder. Configure my_module.
 *
 * @ingroup forms
 * @see system_settings_form()
 */

/**
 * Determine if a machine name exists
 *
 * @param string $machine_name
 *
 * @return bool
 */
function splash_offer_machine_name_exists($machine_name) {
  $found = db_select('splash_offer', 'g')
    ->fields('g', array('machine_name'))
    ->condition('machine_name', $machine_name)
    ->execute()->fetchCol();
  return (bool) count($found);
}

/**
 * Form builder for the splash_offer form.
 *
 * CRUD form for splash_offer entities
 *
 * @see splash_offer_form_validate()
 * @see splash_offer_form_submit()
 * @ingroup forms
 */
function splash_offer_form($form, &$form_state, $entity, $op, $bundle) {
  $form['#splash_offer'] = $entity;
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'splash_offer') . '/splash_offer.js'
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $entity->name,
    '#required' => TRUE,
    '#weight' => -20,
  );

  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine Name'),
    '#default_value' => $entity->machine_name,
    '#maxlength' => MENU_MAX_MENU_NAME_LENGTH_UI,
    '#description' => t('A unique name to construct the URL for the menu. It must only contain lowercase letters, numbers and hyphens.'),
    '#machine_name' => array(
      'exists' => 'splash_offer_machine_name_exists',
      'source' => array('name'),
      'label' => t('Machine Name'),
      'replace_pattern' => '[^a-z0-9_]+',
      'replace' => '_',
    ),
    '#disabled' => FALSE,
    '#weight' => -18,
  );

  //@todo make this a field?
  //@todo add a text filter?
  $form['markup'] = array(
    '#type' => 'textarea',
    '#title' => t('Splash Offer Message or Markup'),
    '#description' => t('Enter the text or markup to present inside the splash offer; <strong>HTML is allowed</strong>.'),
    '#default_value' => $entity->markup,
    '#required' => TRUE,
    '#rows' => 10,
    '#resizable' => TRUE,
  );

  $form['data'] = array(
    '#tree' => TRUE,
  );

  $form['other'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 60,
  );

  $form['other']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active'),
    '#description' => t('To disable a splash offer and hide it from users, uncheck this box.'),
    '#default_value' => $entity->status,
  );

  $form['other']['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#description' => t('In the case of multiple splash offers, lower numbered splash offers will be presented first.'),
    '#default_value' => $entity->weight,
    '#delta' => 20, //the range is from -1 * delta through delta
  );

  $form['data']['links'] = array(
    '#weight' => 10,
    '#type' => 'fieldset',
    '#title' => t('Buttons'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['data']['links']['yes']['text'] = array(
    '#type' => 'textfield',
    '#description' => t('Text to appear on the accept button'),
    '#title' => t('Offer Accept Button'),
    '#default_value' => $entity->links['yes']['text'],
    '#size' => 25,
    '#required' => TRUE,
  );
  $form['data']['links']['yes']['path'] = array(
    '#type' => 'textfield',
    '#description' => t('Enter the URL where the user goes if they accept this offer.'),
    '#title' => t('Offer Accept URL'),
    '#default_value' => $entity->links['yes']['path'],
    '#required' => TRUE,
    '#size' => 100,
  );
  $form['data']['links']['no']['text'] = array(
    '#type' => 'textfield',
    '#description' => t('Text to appear on the decline button.  When the user clicks this button, the offer will disappear.'),
    '#title' => t('Offer Decline Button'),
    '#default_value' => $entity->links['no']['text'],
    '#size' => 25,
    '#required' => TRUE,
  );

  $form['data']['links']['mode'] = array(
    '#name' => '#mode',
    '#type' => 'radios',
    '#title' => t('Button Mode'),
    '#default_value' => $entity->links['#mode'],
    '#options' => array(
      SPLASH_OFFER_BUTTON_MODE_UI    => t('jQuery UI Buttons'),
      SPLASH_OFFER_BUTTON_MODE_HTML  => t('HTML Buttons (.tpl)')
    ),
  );

  $form['data']['storage'] = array(
    '#weight' => 40,
    '#type' => 'fieldset',
    '#title' => t('Repeat Viewing'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['data']['storage']['cookies']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use cookies?'),
    '#description' => t("Should this offer use cookies to know when it's been previously viewed by a user?"),
    '#default_value' => $entity->storage['cookies']['#enabled'],
  );

  $form['data']['storage']['cookies']['fs_cookies'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced (cookies)'),
    '#collapsible' => TRUE,
    //'#collapsed' => !$entity->storage['cookies']['#enabled'],
    '#collapsed' => TRUE,
  );
  $form['data']['storage']['cookies']['fs_cookies']['lifetime'] = array(
    '#type' => 'textfield',
    '#description' => t('How many days before the cookie expires?'),
    '#title' => t('Cookie Lifespan in Days?'),
    '#default_value' => $entity->storage['cookies']['#lifetime'],
    '#required' => FALSE,
    '#size' => 10,
  );
  $form['data']['storage']['cookies']['fs_cookies']['default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Set cookie by default?'),
    '#description' => t("When checked, the user will not have to check the <em>Don't show again</em> option; it will already be checked for them.  Uncheck here for the opposite to be true."),
    '#default_value' => $entity->storage['cookies']['#default'],
  );

  // Per-path visibility.
  $form['data']['audience'] = array(
    '#weight' => 20,
    '#type' => 'fieldset',
    '#title' => t('Pages/Audience'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'visibility',
  );

  $form['data']['audience']['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Which roles should see splash offer?'),
    '#default_value' => array_keys($entity->access['roles']),
    '#options' => user_roles(),
  );

  $access = user_access('use PHP for settings');
  $options = array(
    BLOCK_VISIBILITY_NOTLISTED => t('All pages except those listed'),
    BLOCK_VISIBILITY_LISTED => t('Only the listed pages'),
  );
  $description = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

  if (module_exists('php') && $access) {
    $options += array(BLOCK_VISIBILITY_PHP => t('Pages on which this PHP code returns <code>TRUE</code> (experts only)'));
    $title = t('Pages or PHP code');
    $description .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. Note that executing incorrect PHP code can break your Drupal site.', array('%php' => '<?php ?>'));
  }
  else {
    $title = t('Pages');
  }
  $form['data']['audience']['pages']['visibility'] = array(
    '#type' => 'radios',
    '#title' => t('Show splash offer on specific pages'),
    '#options' => $options,
    '#default_value' => $entity->access['pages']['#visibility'],
  );
  $form['data']['audience']['pages'][0] = array(
    '#type' => 'textarea',
    '#title' => '<span class="element-invisible">' . $title . '</span>',
    '#default_value' => $entity->access['pages'][0],
    '#description' => $description,
  );

  if (module_exists('mobile_detect')
      && ($detect = mobile_detect_get_object())) {
    $form['data']['devices'] = array(
      '#weight' => 100,
      '#type' => 'fieldset',
      '#title' => t('Trigger Devices'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['data']['devices']['always_trigger'] = array(
      '#type' => 'checkbox',
      '#title' => t('Always trigger, regardless of device'),
      '#default_value' => !$entity->access['devices']['#enabled'],
    );
    $form['data']['devices']['fs_phones'] = array(
      '#type' => 'fieldset',
      '#title' => t('Phone Devices'),
      '#collapsible' => TRUE,
      '#collapsed' => !$entity->access['devices']['#enabled'],
    );
    $form['data']['devices']['fs_phones']['phones'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Which phones trigger the splash offer?'),
      '#default_value' => (array) $entity->access['devices']['phones'],
      '#options' => drupal_map_assoc(array_keys($detect->getPhoneDevices())),
    );
    $form['data']['devices']['fs_tablets'] = array(
      '#type' => 'fieldset',
      '#title' => t('Tablet Devices'),
      '#collapsible' => TRUE,
      '#collapsed' => !$entity->access['devices']['#enabled'],
    );
    $form['data']['devices']['fs_tablets']['tablets'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Which tablets trigger the splash offer?'),
      '#default_value' => (array) $entity->access['devices']['tablets'],
      '#options' => drupal_map_assoc(array_keys($detect->getTabletDevices())),
    );
  };

  $form['actions'] = array('#type' => 'actions');
  if (!empty($entity->is_new)) {
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Create Splash Offer'),
    );

  }
  else {
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Splash Offer'),
    );
  }
  return $form;
}

/**
 * Form validation handler for splash_offer_form().
 */
function splash_offer_form_validate($form, &$form_state) {
  if ($form_state['values']['data']['storage']['cookies']['enabled']
      && empty($form_state['values']['data']['storage']['cookies']['fs_cookies']['lifetime'])) {
    form_set_error('data][storage', t('Cookie lifespan must be a positive number when using cookies.'));
  }
}

/**
 * Form submission handler for splash_offer_form().
 *
 * @see splash_offer_form()
 * @see splash_offer_form_validate()
 */
function splash_offer_form_submit($form, &$form_state) {
  $entity = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $entity->save();
  $form_state['redirect'] = 'admin/structure/splash-offer';
}

/** @} */ //end of group splash_offer
