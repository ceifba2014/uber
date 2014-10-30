<?php
/**
 * @file
 * Defines authcache AJAX default request/command handler factory
 * implementation. 
 */

interface AuthcacheAjaxRequestHandler {
  /**
   * Construct a new instance using the given config (key-value map).
   */
  public function __construct($config);

  /**
   * Use the given key-value pairs to generate a list of commands and
   * arguments.
   */
  public function setRequest($req);

  /**
   * Return the number of seconds this response should be cached in the
   * browser. Return NULL otherwise.
   */
  public function getCacheMaxAge();

  /**
   * Return a list of commands keyed by their opcodes.
   */
  public function getCommands();

  /**
   * Return a map of opcodes to their results.
   */
  public function getResponse($results);
}
