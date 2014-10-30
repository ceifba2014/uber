<?php
/**
 * @file
 * Rules integration for uc_coupon_purchase
 */

/**
 * Implements hook_rules_event_info().
 */
function uc_coupon_purchase_rules_event_info() {
  $events = array();
  $events['uc_coupon_purchase'] = array(
    'label' => t('Customer purchases a coupon'),
    'group' => t('Coupon'),
    'variables' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
      ),
      'coupon' => array(
        'type' => 'uc_coupon',
        'label' => t('Coupon'),
      ),
      'product' => array(
        'type' => 'uc_order_product',
        'label' => t('The purchased product of which the coupon is a feature.'),
      ),
    ),
  );
  return $events;
}


/**
 * Implements hook_rules_action_info().
 */
function uc_coupon_purchase_rules_action_info() {
  $actions = array();
  $actions['uc_coupon_purchase_email'] = array(
    'label' =>  t('Send an order email regarding coupon purchase.'),
    'group' => t('Coupon'),
    'parameter' => array(
      'order' => array(
        'type' => 'uc_order',
        'label' => t('Order'),
        'optional' => TRUE,
      ),
      'coupon' => array(
        'type' => 'uc_coupon',
        'label' => t('Coupon'),
      ),
      'from' => array(
        'type' => 'text',
        'label' => t('Sender'),
      ),
      'addresses' => array(
        'type' => 'text',
        'label' => t('Recipients'),
        'description' => t('Enter the email addresses to receive the notifications, one on each line. You may use order tokens for dynamic email addresses.'),
      ),
      'subject' => array(
        'type' => 'text',
        'label' => t('Subject'),
      ),
      'message' => array(
        'type' => 'text',
        'label' => t('Message'),
      ),
      'format' => array(
        'type' => 'text',
        'label' => t('Message format'),
        'options list' => 'uc_coupon_purchase_message_formats',
      ),

    ),
  );

  $actions['uc_coupon_purchase_assign_action'] = array(
    'label' =>  t('Assign a coupon to a user'),
    'group' => t('Coupon'),
    'provides' => array(
      'coupon' => array(
        'type' => 'uc_coupon',
        'label' => t('Coupon'),
      ),
    ),
    'named parameter' => TRUE,

    'parameter' => array(
      'cid' => array(
        'type' => 'integer',
        'label' => t('Coupon'),
        'description' => t('Select the base coupon to be assigned.'),
        'options list' => 'uc_coupon_purchase_cid_options',
      ),
      'account' => array(
        'type' => 'user',
        'label' => t('Account'),
        'description' => t('Select the user to whom the coupon should be assigned.'),
      ),
      'restrict' => array(
        'type' => 'boolean',
        'label' => t('Restrict coupon usage to assigned users.'),
        'description' => t('When selected, the assignee will be added to the list of authorized
          users for this coupon.  Only authorized users will be able to use the coupon.'),
        'default value' => FALSE,
        'allow null' => TRUE,
        'restriction' => 'input',
        'optional' => TRUE,
      ),
      'sole' => array(
        'type' => 'boolean',
        'label' => t('Make this user the sole owner.'),
        'description' => t('When selected, all other assigned users will be removed.
          If you check "%r" above, this user will become the only person authorized to use this coupon.', array(
            '%r' => t('Restrict coupon usage to assigned users.')
        )),
        'default value' => FALSE,
        'allow null' => TRUE,
        'restriction' => 'input',
        'optional' => TRUE,
      ),
      'clone' => array(
        'type' => 'boolean',
        'label' => t('Clone the base coupon before assigning.'),
        'description' => t('When checked, the base coupon will be cloned and a copy assigned to the specified user,
          exactly as if the user had purchased the coupon.  If unchecked, the base coupon will be assigned to the
          user directly'),
        'default value' => FALSE,
        'allow null' => TRUE,
        'restriction' => 'input',
        'optional' => TRUE,
      ),
      'quantity' => array(
        'type' => 'integer',
        'label' => t('Quantity.'),
        'description' => t('The number of copies of the base coupon to assign. If the base coupon is a bulk coupon,
          this will be multiplied by the base coupon\'s bulk number. Only meaningful if you select "%c" above.', array(
            '%c' => t('Clone the base coupon before assigning.'),
        )),
        'default value' => 1,
        'allow null' => TRUE,
        'restriction' => 'input',
        'optional' => TRUE,
      ),

    ),
  );

  return $actions;
}

/**
 * Rules form_alter callback for the uc_coupon_purchase_assign_action.
 */
function uc_coupon_purchase_assign_action_form_alter(&$form, &$form_state) {
  if (isset($form['parameter']['cid']['settings']['cid'])) {
    // Add a set of radio buttons to limit the choice of coupons by status.
    $form['parameter']['cid']['settings']['cid']['#prefix'] = '<div id="uc-coupon-purchase-assign-cid">';
    $form['parameter']['cid']['settings']['cid']['#suffix'] = '</div>';
    //$cid_type = empty($form_state['values']['parameter']['cid']['settings']['cid_type']) ?
    //  0 : $form_state['values']['parameter']['cid']['settings']['cid_type'];
    //$form['parameter']['cid']['settings']['cid']['#options'] = uc_coupon_purchase_cid_options($cid_type);
    $form['parameter']['cid']['settings']['cid_type'] = array(
      '#type' => 'radios',
      '#title' => t('Choose from'),
      '#options' => array(
         0 => t('All coupons'),
         1 => t('Inactive coupons'),
         2 => t('Active coupons'),
      ),
      '#default_value' => 0,
      '#ajax' => array(
        'callback' => 'uc_coupon_purchase_assign_action_form_ajax',
        'wrapper' => 'uc-coupon-purchase-assign-cid',
      ),
    );
  }
  elseif (isset($form['parameter']['cid']['settings']['cid:select'])) {
    $form['parameter']['cid']['#description'] = t('Select the base coupon to be assigned. Note, you must choose the
      ID of the coupon (i.e. "%selector")', array('%selector' => 'coupon:cid'));
  }
  // Hide the "quantity"  ofield when not applicable.
  $form['parameter']['quantity']['#states'] = array(
    'invisible' => array(
      'input[name="parameter[clone][settings][clone]"]' => array('checked' => FALSE),
    )
  );
}

/**
 * Ajax callback for the uc_coupon_purchase_assign action.
 */
function uc_coupon_purchase_assign_action_form_ajax($form, $form_state) {
  return $form['parameter']['cid']['settings']['cid'];
}

/**
 * Options list callback for message formats.
 */
function uc_coupon_purchase_message_formats() {
  global $user;

  $options = array();
  $formats = filter_formats($user);
  foreach ($formats as $format) {
    $options[$format->format] = $format->name;
  }
  return $options;
}


/**
 * Action callback to send an e-mail regarding coupon purchase.
 */
function uc_coupon_purchase_email($order, $coupon, $from, $addresses, $subject, $message, $format) {
  $settings = array(
    'from' => $from,
    'subject' => $subject,
    'message' => $message,
    'format' => $format,
    'replacements' => array(),  // Replacements are handled directly by Rules.
  );

  // Split up our recipient e-mail addresses.
  $recipients = array();
  foreach (explode("\n", $addresses) as $address) {
    $recipients[] = trim($address);
  }

  // Send the e-mails.
  foreach ($recipients as $email) {
    $sent = drupal_mail('uc_order', 'action-mail', $email, uc_store_mail_recipient_language($email), $settings, empty($settings['from']) ? uc_store_email_from() : $settings['from']);
    if (!$sent['result']) {
      watchdog('uc_order', 'Attempt to e-mail @email concerning order @order_id failed.', array('@email' => $email, '@order_id' => $order->order_id), WATCHDOG_ERROR);
    }
  }
}

/**
 * Action callback to assign a coupon to a user.
 */
function uc_coupon_purchase_assign_action($params) {
  $coupon = uc_coupon_load($params['cid']);
  if ($coupon->cid) {
    if (!empty($params['clone'])) {
      $coupon = uc_coupon_purchase_create($coupon, $params['quantity'], $params['account']->uid);
    }
    elseif (!db_query('SELECT cpuid FROM {uc_coupon_purchase_users} where uid = :uid AND cid = :cid',
        array(':uid' => $params['account']->uid, ':cid' => $coupon->cid))->fetchField(0)) {
      $record = array('uid' => $params['account']->uid, 'cid' => $coupon->cid);
      drupal_write_record('uc_coupon_purchase_users', $record);
    }
    if (!empty($params['sole'])) {
      db_delete('uc_coupon_purchase_users')
      ->condition('cid', $coupon->cid)
      ->condition('uid', $params['account']->uid, '!=')
      ->execute();
    }
    if (!empty($params['restrict'])) {
      if (!empty($params['sole'])) {
        $coupon->data['users'] = array();
      }
      $coupon->data['users'][$params['account']->uid] = $params['account']->uid;
      uc_coupon_save($coupon);
    }
    return array('coupon' => $coupon);
  }
  else {
    return array('coupon' => NULL);
  }
}

/**
 * Options list callback for list of coupons to assign.
 */
function uc_coupon_purchase_cid_options($action) {
  if (empty($action->settings['cid_type'])) {
    return db_query('SELECT cid, name FROM {uc_coupons}')->fetchAllKeyed();
  }
  else {
    $status = $action->settings['cid_type'] - 1;
    return db_query('SELECT cid, name FROM {uc_coupons} where status = :status', array(':status' => $status))->fetchAllKeyed();
  }
}
