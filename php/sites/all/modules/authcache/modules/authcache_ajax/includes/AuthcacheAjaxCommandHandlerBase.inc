<?php
/**
 * @file
 * Defines authcache AJAX command handler interface.
 */

/**
 * Authcache AJAX command handler.
 */
abstract class AuthcacheAjaxCommandHandlerBase {
  /**
   * Prepare the command. This is commonly used to take drupal to the required
   * bootstrap phase.
   */
  public function prepare($op, $config) {
    if (!empty($config['bootstrap'])) {
      // Do not bother setting the recursion-prevention flag here, we will exit
      // before getting back to the parent drupal_bootstrap frame.
      drupal_bootstrap($config['bootstrap']);
    }
  }
}
