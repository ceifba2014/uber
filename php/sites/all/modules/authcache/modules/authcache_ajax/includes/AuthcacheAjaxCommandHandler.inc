<?php
/**
 * @file
 * Defines authcache AJAX command handler interface.
 */

/**
 * Authcache AJAX command handler.
 */
interface AuthcacheAjaxCommandHandler {
  /**
   * Prepare the command. This is commonly used to take drupal to the required
   * bootstrap phase.
   */
  public function prepare($op, $config);

  /**
   * Execute the command with the given opcode and arguments and return its
   * results.
   */
  public function run($op, $args);
}
