<?php
/**
 * @file
 * Entity Controller and Metadata Controller classes for uc_coupon.
 */

/**
 * Controller class for uc_coupon entity.
 */
class UcCouponController extends EntityAPIController {
  /**
   * @see EntityAPIController::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    parent::delete($ids, $transaction);
    db_delete('uc_coupons_orders')
    ->condition('cid', $ids, 'IN')
    ->execute();
  }
  
  /**
   * @see EntityAPIController::save()
   */
  public function save($coupon, DatabaseTransaction $transaction = NULL) {
    if (empty($coupon->cid)) {
      $coupon->created = REQUEST_TIME;
      $coupon->bulk_seed = md5(uniqid());
    }
    parent::save($coupon, $transaction);
  }

  /**
   * @see EntityAPIController::buildContent()
   */
  public function buildContent($coupon, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $rows = array();
    $rows[] = array(t('Name'), check_plain($coupon->name));

    if (!$coupon->status) {
      $status = t('Inactive');
    }
    elseif (!$coupon->valid_until) {
      $status = t('Active');
    }
    elseif (REQUEST_TIME < $coupon->valid_from) {
      $status = t('Not valid until @date', array('@date' => _uc_coupon_format_date($coupon->valid_from, variable_get('date_format_uc_store', 'm/d/Y H:iT'))));
    }
    elseif (REQUEST_TIME > $coupon->valid_until) {
      $status = t('Expired on @date', array('@date' => _uc_coupon_format_date($coupon->valid_until, variable_get('date_format_uc_store', 'm/d/Y H:iT'))));
    }
    else {
      $status = t('Active until @date', array('@date' => _uc_coupon_format_date($coupon->valid_until, variable_get('date_format_uc_store', 'm/d/Y H:iT'))));
    }
    $rows[] = array(t('Status'), $status);

    if (!$coupon->bulk) {
      $rows[] = array(t('Code'), check_plain($coupon->code) . ' (' . l(t('Print'), 'admin/store/coupons/' . $coupon->cid . '/print') . ')');
      if ($coupon->max_uses) {
        $rows[] = array(t('Maximum uses'), $coupon->max_uses);
      }
    }
    else {
      $codes = '<strong>' . check_plain($coupon->code) . ' &times; ' . check_plain($coupon->data['bulk_number']) . '</strong>';
      $codes .= ' (' . l(t('Download codes'), 'admin/store/coupons/' . $coupon->cid . '/codes') . ')';
      $codes .= ' (' . l(t('Print all'), 'admin/store/coupons/' . $coupon->cid . '/print') . ')';
      for ($id = 0; $id < $coupon->data['bulk_number']; $id++) {
        $code = uc_coupon_get_bulk_code($coupon, $id);
        $codes .= '<br />' . check_plain($code) . ' (' . l(t('Print'), 'admin/store/coupons/' . $coupon->cid . '/print/' . $code) . ')';
      }
  
      $rows[] = array(t('Codes'), $codes);
      //$rows[] = array(t('Bulk seed'), check_plain($coupon->bulk_seed));
      if ($coupon->max_uses) {
        $rows[] = array(t('Maximum uses per code'), $coupon->max_uses);
      }
    }

    $rows[] = array(t('Discount value'), theme('uc_coupon_discount', array('coupon' => $coupon)));

    switch ($coupon->data['apply_to']) {
      case 'subtotal':
        $applies = t('Order subtotal');
        break;
      case 'products_total':
        $applies = t('Total of matching products');
        break;
      case 'products':
        $applies = t('Matching products');
        break;
      case 'cheapest':
        $applies = format_plural($coupon->data['apply_count'], 'Cheapest product', '@count cheapest products');
        break;
      case 'expensive':
        $applies = format_plural($coupon->data['apply_count'], 'Most expensive product', '@count most expensive products');
        break;
    }
    $rows[] = array(t('Applied to'), $applies);

    if ($coupon->data['apply_to'] != 'subtotal') {
      $restrict = array();
      if (isset($coupon->data['product_types'])) {
        $key = format_plural(count($coupon->data['product_types']), 'All products in class', 'All products in classes');
        $restrict[$key] = $coupon->data['product_types'];
      }
      if (isset($coupon->data['products'])) {
        $products = array();
        foreach ($coupon->data['products'] as $nid) {
          $products[] = check_plain(db_query("SELECT title FROM {node} WHERE nid = :nid", array(':nid' => $nid))->fetchField());
        }
        if (isset($coupon->data['negate_products'])) {
          $restrict[t('All products except')] = $products;
        }
        else {
          $restrict[format_plural(count($products), 'Product', 'Products')] = $products;
        }
      }
      if (isset($coupon->data['skus'])) {
        $restrict[format_plural(count($coupon->data['skus']), 'SKU', 'SKUs')] = $coupon->data['skus'];
      }
      if (isset($coupon->data['terms'])) {
        $terms = array();
        foreach ($coupon->data['terms'] as $tid) {
          $terms[] = check_plain(db_query("SELECT name FROM {taxonomy_term_data} WHERE tid = :tid", array(':tid' => $tid))->fetchField());
        }
        if (isset($coupon->data['negate_terms'])) {
          $restrict[t('All taxonomy terms except')] = $terms;
        }
        else {
          $restrict[format_plural(count($terms), 'Taxonomy term', 'Taxonomy terms')] = $terms;
        }
      }
      if ($restrict) {
        $or = FALSE;
        foreach ($restrict as $title => &$restriction) {
          if ($or) {
            $title = t('or') . ' ' . $title;
          }
          $restriction = $title . ': <em>' . implode('</em>, <em>', $restriction) . '</em>';
          $or = TRUE;
        }
        $rows[] = array(t('Product restrictions'), implode($restrict, '<br />'));
      }
    }

    $restrict = array();
    if (isset($coupon->data['users'])) {
      $users = array();
      foreach ($coupon->data['users'] as $uid) {
        $users[] = check_plain(db_query("SELECT name FROM {users} WHERE uid = :uid", array(':uid' => $uid))->fetchField());
      }
      if (isset($coupon->data['negate_users'])) {
        $restrict[t('All users except')] = $users;
      }
      else {
        $restrict[format_plural(count($users), 'User', 'Users')] = $users;
      }
    }
    if (isset($coupon->data['max_uses_per_user'])) {
      $restrict['Maximum uses per user'] = array($coupon->data['max_uses_per_user']);
    }
    if (isset($coupon->data['roles'])) {
      if (isset($coupon->data['negate_roles'])) {
        $restrict[t('All roles except')] = $coupon->data['roles'];
      }
      else {
        $restrict[format_plural(count($coupon->data['roles']), 'Role', 'Roles')] = $coupon->data['roles'];
      }
    }
    if ($restrict) {
      foreach ($restrict as $title => &$restriction) {
        $restriction = $title . ': <em>' . implode('</em>, <em>', $restriction) . '</em>';
      }
      $rows[] = array(t('User restrictions'), implode($restrict, '<br />'));
    }

    if ($coupon->minimum_order > 0) {
      $rows[] = array(t('Order restrictions'), t('Minimum subtotal') . ': <em>' . uc_currency_format($coupon->minimum_order) . '</em>');
    }

    $combo_list = array();
    if (!empty($coupon->data['combinations'])) {
      $combos = db_query('SELECT cid, name FROM {uc_coupons} where cid IN (:cids)', array(':cids' => $coupon->data['combinations']));
      foreach ($combos as $combo) {
        $combo_list[] = check_plain("$combo->name [cid:$combo->cid]");
      }
    }
    if (isset($coupon->data['negate_combinations'])) {
      $ctext = empty($combo_list) ? t('Any.') : t('Any but:') . ' ' . implode(', ', $combo_list);
    }
    else {
      $ctext = empty($combo_list) ? t('None.') : t('Only:') . ' ' . implode(', ', $combo_list);
    }
    $rows[] = array(t('Allowed Combinations'), $ctext);

    foreach ($rows as &$row) {
      $row[0] = array(
        'header' => TRUE,
        'data' => $row[0],
      );
    }

    $content['admin_summary'] = array(
      '#title' => t('Administrative Summary'),
      '#theme' => 'table',
      '#rows' => $rows
    );

    return parent::buildContent($coupon, $view_mode, $langcode, $content);
  }
}

/**
 * Metadata Controller for uc_coupon entity
 */
class UcCouponMetadataController extends EntityDefaultMetadataController {
  public function entityPropertyInfo() {
    $prop_info = parent::entityPropertyInfo();
    $props = &$prop_info['uc_coupon']['properties'];
    //dpm($prop_info);
    // Copy the descriptions from the schema. Drupal discards this information, so we have to
    // call uc_order_schema() directly.
    module_load_include('install', 'uc_coupon', 'uc_coupon');
    $schema = uc_coupon_schema();
    foreach ($schema['uc_coupons']['fields'] as $name => $info) {
      if (is_array($props[$name]) && !empty($info['description'])) {
        $props[$name]['description'] = $info['description'];
      }
    }

    $props['type']['options list'] = '_uc_coupon_type_options';
    $props['status']['options list']['type'] = 'boolean';
    $props['bulk']['type'] = 'boolean';

    // Set the correct type for the date properties.
    foreach (array('created', 'valid_from', 'valid_until') as $key) {
      $props[$key]['type'] = 'date';
      $props[$key]['getter callback'] = 'entity_property_verbatim_date_get';
    }

    // Unpack some of the 'data' properties.
    unset($props['data']);
    $props['products'] = array(
      'type' => 'list<node>',
      'label' => t('Products'),
      'description' => t('The applicable products for this coupon.'),
      'getter callback' => 'uc_coupon_data_property_get',
      'setter callback' => 'uc_coupon_data_property_set',
    );
    $props['negate_products'] = array(
      'type' => 'boolean',
      'label' => t('Negate applicable products'),
      'description' => t('Whether or not the products list represents allowed or disallowed products'),
      'getter callback' => 'uc_coupon_data_property_get',
      'setter callback' => 'uc_coupon_data_property_set',
    );
    $props['base_coupon'] = array(
      'type' => 'uc_coupon',
      'description' => t('The coupon on which a purchased or assigned coupon is based'),
      'label' => t('Base coupon'),
      'getter callback' => 'uc_coupon_data_property_get',
      'setter callback' => 'uc_coupon_data_property_set'
    );
    return $prop_info;
  }
}

/**
 * Generic getter to extract properties from the coupon data array.
 */
function uc_coupon_data_property_get($coupon, array $options, $name, $entity_type) {
  switch ($name) {
    case 'base_coupon':
      return empty($coupon->data['base_cid']) ? FALSE : uc_coupon_load($coupon->data['base_cid']);
      break;
    default:
      if (!isset($coupon->data[$name])) {
        return NULL;
      }
      elseif (is_array($coupon->data[$name])) {
        return array_values($coupon->data[$name]);
      }
      else {
        return $coupon->data[$name];
      }
  }
}

/**
 * Generic setter to update properties from the coupon data array.
 */
function uc_coupon_data_property_set($coupon, $name, $value) {
  switch ($name) {
    case 'base_coupon':
      $coupon->data[$name] = $value->cid;
      break;
    default:
      if (is_array($value)) {
        $coupon->data[$name] = drupal_map_assoc($value);
      }
      else {
        $coupon->data[$name] = $name;
      }
  }
}

/**
 * Options callback for the coupon type property.
 */
function _uc_coupon_type_options() {
  return array(
    'price' => t('Fixed discount'),
    'percentage' => t('Percentage'),
    'set_price' => t('Set product price'),
    'credit' => t('Store credit')
  );
}

/**
 * Options callback for the coupon status property
 */
function _uc_coupon_status_options() {
  return array(
    0 => t('Inactive'),
    1 => t('Active'),
  );
}
