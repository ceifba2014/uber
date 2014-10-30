<?php
/**
 * @file
 * Defines authcache AJAX default request/command handler factory
 * implementation. 
 */

class AuthcacheAjaxDefaultRequestHandler {
  private $maxage = NULL;
  private $req = array();

  /**
   * Construct new AjaxDefaultRequestHandler.
   */
  public function __construct($config) {
    if (!empty($config['maxage'])) {
      $this->maxage = $config['maxage'];
    }
  }

  /**
   * Use the given key-value pairs to generate a list of commands and
   * arguments.
   */
  public function setRequest($req) {
    $this->req = $req;  
  }

  /**
   * Return the number of seconds this response should be cached in the
   * browser. Return NULL otherwise.
   */
  public function getCacheMaxAge() {
    if ($this->maxage) {
      return $this->maxage;
    }
  }

  /**
   * Return a list of commands keyed by their opcodes.
   */
  public function getCommands() {
    $strip = array(
      'q' => TRUE,
    );

    return array_diff_key($this->req, $strip);
  }

  /**
   * Return a map of opcodes to their results.
   */
  public function getResponse($results) {
    return $results;
  }
}
