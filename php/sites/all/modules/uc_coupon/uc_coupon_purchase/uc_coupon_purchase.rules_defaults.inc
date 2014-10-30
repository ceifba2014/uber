<?php
/**
 * @file
 * Default Rules configurations for uc_coupon_purchase.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function uc_coupon_purchase_default_rules_configuration() {
  $rule = rules_reaction_rule();
  $rule->label = t('Notify customer when a single coupon is purchased.');
  $rule->active = TRUE;
  $rule->event('uc_coupon_purchase')
    ->condition(rules_condition('uc_coupon_condition_is_bulk', array('coupon:select' => 'coupon'))->negate())
    ->action('uc_coupon_purchase_email', array(
      'order:select' => 'order',
      'coupon:select' => 'coupon',
      'from' => uc_store_email_from(),
      'addresses' => '[order:email]',
      'subject' => t('[store:name]: coupon purchase'),
      'message' => t("[order:first-name] [order:last-name], \n\nThanks to your order, [order:link], at [store:name] you now have a new coupon available to use with the code [coupon:codes].\n\nThanks again, \n\n[store:name]\n[site:slogan]"),
      'format' => filter_default_format(),
    ));

  $configs['uc_coupon_purchase_single'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Notify customer when a bulk coupon is purchased.');
  $rule->active = TRUE;
  $rule->event('uc_coupon_purchase')
    ->condition(rules_condition('uc_coupon_condition_is_bulk', array('coupon:select' => 'coupon')))
    ->action('uc_coupon_purchase_email', array(
      'order:select' => 'order',
      'coupon:select' => 'coupon',
      'from' => uc_store_email_from(),
      'addresses' => '[order:email]',
      'subject' => t('[store:name]: bulk coupon purchase'),
      'message' => t("[order:first-name] [order:last-name], \n\nThanks to your order, [order:link], at [store:name] you now have a new set of coupons available to use with the following codes:\n\n[coupon:codes]\n\nThanks again, \n\n[store:name]\n[site:slogan]"),
      'format' => filter_default_format(),
    ));

  $configs['uc_coupon_purchase_bulk'] = $rule;

  $configs['uc_coupon_gift_single'] = rules_import('{ "uc_coupon_gift_single" : {
      "LABEL" : "Notify recipient when a single coupon is purchased as a gift.",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "uc_coupon", "rules", "uc_coupon_purchase" ],
      "ON" : [ "uc_coupon_purchase" ],
      "IF" : [
        { "NOT uc_coupon_condition_is_bulk" : { "coupon" : [ "coupon" ] } },
        { "NOT data_is_empty" : { "data" : [ "product:uc-coupon-purchase-recipient" ] } }
      ],
      "DO" : [
        { "uc_coupon_purchase_email" : {
            "order" : [ "order" ],
            "coupon" : [ "coupon" ],
            "from" : [ "order:primary-email" ],
            "addresses" : [ "product:uc-coupon-purchase-recipient" ],
            "subject" : "[store:name]: Gift Certificate",
            "message" : "Hello!\r\n\r\n[order:first-name] [order:last-name] has sent you a gift certificate worth [coupon:value] for use at [store:name].\r\n\r\nJust enter the code [coupon:code] at checkout to receive your discount.\r\n\r\nThanks,\r\n\r\n[store:name]\r\n[site:slogan]",
            "format" : "filtered_html"
          }
        }
      ]
    }
  }');

  $configs['uc_coupon_gift_multiple'] = rules_import('{ "uc_coupon_gift_multiple" : {
      "LABEL" : "Notify recipient when a bulk coupon is purchased as a gift.",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "uc_coupon", "rules", "uc_coupon_purchase" ],
      "ON" : [ "uc_coupon_purchase" ],
      "IF" : [
        { "uc_coupon_condition_is_bulk" : { "coupon" : [ "coupon" ] } },
        { "NOT data_is_empty" : { "data" : [ "product:uc-coupon-purchase-recipient" ] } }
      ],
      "DO" : [
        { "uc_coupon_purchase_email" : {
            "order" : [ "order" ],
            "coupon" : [ "coupon" ],
            "from" : [ "order:primary-email" ],
            "addresses" : [ "product:uc-coupon-purchase-recipient" ],
            "subject" : "[store:name]: Gift Certificate",
            "message" : "Hello!\r\n\r\n[order:first-name] [order:last-name] has sent you a set of gift certificates, each worth [coupon:value], for use at [store:name].\r\n\r\nJust enter one of the codes below at checkout to receive your discount.\r\n\r\n[coupon:codes]\r\n\r\nThanks,\r\n\r\n[store:name]\r\n[site:slogan]",
            "format" : "filtered_html"
          }
        }
      ]
    }
  }');
  
  $configs['uc_coupon_assign_on_checkout'] = rules_import('{ "uc_coupon_assign_on_checkout" : {
      "LABEL" : "Assign a store-credit coupon to its user on checkout",
      "PLUGIN" : "reaction rule",
      "ACTIVE" : false,
      "REQUIRES" : [ "rules", "uc_coupon_purchase", "uc_coupon_workflow" ],
      "ON" : [ "uc_coupon_workflow_checkout" ],
      "IF" : [ { "data_is" : { "data" : [ "coupon:type" ], "value" : "credit" } } ],
      "DO" : [
        { "uc_coupon_purchase_assign_action" : {
            "USING" : {
              "cid" : [ "coupon:cid" ],
              "account" : [ "order:customer" ],
              "restrict" : 1,
              "sole" : 1
            },
            "PROVIDE" : { "coupon" : { "assigned_coupon" : "Coupon" } }
          }
        }
      ]
    }
  }');
  
  return $configs;
}