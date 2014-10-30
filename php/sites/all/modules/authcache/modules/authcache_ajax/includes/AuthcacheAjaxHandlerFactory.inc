<?php
/**
 * @file
 * Defines authcache AJAX request/command handler factory interface and default
 * implementation.
 */

/**
 * Factory class for authcache AJAX request and command handlers.
 */
interface AuthcacheAjaxHandlerFactory {
  /**
   * Return an AuthcacheAjaxRequestHandler instance capable of processing the given
   * request.
   */
  public function getRequestHandler($req);

  /**
   * Return an AuthcacheAjaxCommandHandler instance capable of executing the actions
   * defined by the given opcode.
   */
  public function getCommandHandler($op);

  /**
   * Return info structure about all defined requests.
   */
  public function getRequestInfo();

  /**
   * Return info structure about all defined commands.
   */
  public function getCommandInfo();

  /**
   * Rebuild info structure for requests and commands.
   */
  public function rebuildInfo();
}
