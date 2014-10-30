<?php

/**
 * Retrieve CSRF-form tokens for the logged in user.
 *
 * @see form.inc
 */
class AuthcacheFormTokenIdCommand extends AuthcacheAjaxCommandHandlerBase {
  public function run($op, $forms) {
    include_once DRUPAL_ROOT . '/includes/common.inc';
    foreach ($forms as $form_id => $form_token_id) {
      $tokens[$form_id] = drupal_get_token($form_token_id);
    }
    return $tokens;
  }
}
