<?php
/**
 * @file
 * Page callbacks for uc_coupon.
 */

/**
 * Display a list of purchased coupons.
 */
function uc_coupon_purchase_view($account) {
  drupal_set_title(t('My coupons and credits'));

  $result = db_query('SELECT c.* FROM {uc_coupon_purchase_users} u INNER JOIN {uc_coupons} c ON u.cid = c.cid WHERE u.uid = :uid ORDER BY valid_until ASC, code ASC', array(':uid' => $account->uid));

  $header = array(t('Code'), t('Value'), t('Validity'), t('Used'), t('Remaining'));
  $rows = array();
  foreach ($result as $coupon) {
    if ($coupon->bulk) {
      $coupon->data = unserialize($coupon->data);
      $codes = array(); 
      for ($id = 0; $id < $coupon->data['bulk_number']; $id++) {
        $codes[] = uc_coupon_get_bulk_code($coupon, $id);
      }
    }
    else {
      $codes = array($coupon->code);
    }

    $value = theme('uc_coupon_discount', array('coupon' => $coupon));
    if (!$coupon->status) {
      $valid = t('Inactive');
    }
    elseif (!$coupon->valid_until) {
      $valid = t('Unlimited');
    }
    else {
      $valid_from = format_date($coupon->valid_from, 'custom', variable_get('date_format_uc_store', 'm/d/Y'));
      $valid_until = format_date($coupon->valid_until, 'custom', variable_get('date_format_uc_store', 'm/d/Y'));
      $valid = $valid_from . ' - ' . $valid_until;
    }
    $usage = uc_coupon_count_usage($coupon->cid);

    $icon = theme('image', array('path' => drupal_get_path('module', 'uc_store') . '/images/print.gif', 'width' => t('Print')));
    foreach ($codes as $code) {
      if (substr($code, -1) == '*') {
        // For the base code of bulk coupons, just give a link to print all.
        $title = t('All %code', array('%code' => $code));
        $link = l($title, 'user/' . $account->uid . '/coupons/' . $coupon->cid);
        $link .= ' ' . l($icon, 'user/' . $account->uid . '/coupons/' . $coupon->cid . '/print', array('html' => TRUE));
        $rows[] = array($link, '', '', '', '');
      }
      else {
        $link = l($code, 'user/' . $account->uid . '/coupons/' . $coupon->cid . '/view/' . $code);
        $link .= ' ' . l($icon, 'user/' . $account->uid . '/coupons/' . $coupon->cid . '/print/' . $code, array('html' => TRUE));
        if ($coupon->type === 'credit') {
          $used = empty($usage['value']['codes'][$code]) ? 0 : $usage['value']['codes'][$code];
          $remaining = uc_currency_format($coupon->value - $used);
          $used = uc_currency_format($used);
        }
        else {
          $used = empty($usage['codes'][$code]) ? 0 : $usage['codes'][$code];
          $remaining = $coupon->max_uses > 0 ? $coupon->max_uses - $used : t('Unlimited');
        }
        $rows[] = array($link, $value, $valid, $used, $remaining);
      }
    }
  }

  if (count($rows)) {
    $output = "<p>" . t("The table below lists the discount coupons and credits available to you.") . "</p>";
    $output .= theme('table', array('header' => $header, 'rows' => $rows));
  }
  else {
    $output = "<p>" . t('You currently have no coupons or credits available.') . "</p>";
  }

  return $output;
}

/**
 * Print a purchased coupon.
 */
function uc_coupon_purchase_print($account, $coupon, $op = 'view', $code = NULL) {
  drupal_set_title(t('Coupon #@cid', array('@cid' => $coupon->cid)), PASS_THROUGH);

  module_load_include('inc', 'uc_coupon', 'uc_coupon.admin');

  $url = 'user/' . $account->uid . '/coupons/' . $coupon->cid . '/print' . ($code ? "/$code" : '');
  $output = '<p>' . l($coupon->bulk && !$code ? t('Print coupons') : t('Print coupon'), $url) . '</p>';

  // Add the owner account object to the coupon.
  $coupon->owner = $account;

  // Does not return if op = 'print'
  $output .= uc_coupon_print($coupon, $code, $op);

  return $output;
}
