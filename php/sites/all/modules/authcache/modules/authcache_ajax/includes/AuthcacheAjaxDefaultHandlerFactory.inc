<?php
/**
 * @file
 * Defines authcache AJAX default request/command handler factory
 * implementation. 
 */

/**
 * Default implementation of AuthcacheAjaxHandlerFactory.
 */
class AuthcacheAjaxDefaultHandlerFactory implements AuthcacheAjaxHandlerFactory {
  private $commands = array();

  /**
   * Lookup handler in registry and return new instance.
   */
  public function getRequestHandler($req) {
    // Default class and config
    $class = 'AuthcacheAjaxDefaultRequestHandler';
    $config = array();

    // If provided extract the cache group and map it to the proper handler.
    if (!empty($req['g'])) {
      list($class, $config) = $this->lookupRequestClassConfig($req['g'], $class, $config);
    }

    // Parameters used for cache group and cache version.
    unset($req['g']);
    unset($req['v']);

    // Configure handler and return it.
    $handler = new $class($config);
    $handler->setRequest($req);

    return $handler;
  }

  /**
   * Lookup command in registry and return new handler instance.
   */
  public function getCommandHandler($op) {
    list($class, $config) = $this->lookupCommandClass($op, NULL, array());

    if ($class) {
      if (!isset($this->commands[$class])) {
        $this->commands[$class] = new $class();
      }

      $this->commands[$class]->prepare($op, $config);

      return $this->commands[$class];
    }
  }

  /**
   * Return info structure about all defined requests.
   */
  public function getRequestInfo() {
    $registry = $this->getRegistry();
    return $registry['requests'];
  }

  /**
   * Return info structure about all defined commands.
   */
  public function getCommandInfo() {
    $registry = $this->getRegistry();
    return $registry['commands'];
  }

  /**
   * Rebuild info structure for requests and commands.
   */
  public function rebuildInfo() {
    // Collect all command definitions
    $commands = module_invoke_all('authcache_ajax_command');
    drupal_alter('authcache_ajax_command', $commands);

    // Prepare a set of default request definitions by examining request config
    // given by defined commands.
    $command_requests = array();
    foreach ($commands as $key => $item) {
      if (empty($item['request'])) {
        $commands[$key]['request'] = 'default';
      }

      $type = $commands[$key]['request'];

      if (!isset($command_requests[$type])) {
        $command_requests[$type] = array();
      }

      if (!empty($item['request config'])) {
        $prev_config = array();
        if (!empty($command_requests[$type])) {
          $prev_config = $command_requests[$type];
        }

        $command_requests[$type] = $prev_config + $item['request config'];
      }
    }

    // Collect all request definitions
    $collected_requests = module_invoke_all('authcache_ajax_request');
    $requests = drupal_array_merge_deep($command_requests, $collected_requests);
    drupal_alter('authcache_ajax_request', $requests);

    $registry = array(
      'commands' => $commands,
      'requests' => $requests,
    );

    cache_set('authcache_ajax_registry', $registry, 'cache_bootstrap');

    return $registry;
  }

  /**
   * Return the internal request/command registry
   */
  protected function getRegistry() {
    $cache = cache_get('authcache_ajax_registry', 'cache_bootstrap');

    if ($cache === FALSE) {
      drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
      $registry = $this->rebuildInfo();
    }
    else {
      $registry = $cache->data;
    }

    return $registry;
  }

  /**
   * Given a group, return the associated classname and configuration as an
   * array. Return the given class and config if the registry does not contain
   * one which is better suited.
   */
  protected function lookupRequestClassConfig($group, $class, $config) {
    $request_info = $this->getRequestInfo();
    if (!empty($request_info[$group])) {
      $config = $request_info[$group];
      if (isset($config['class'])) {
        $class = $config['class'];
        unset($config['class']);
      }
    }

    return array($class, $config);
  }


  /**
   * Given an operation, return the associated classname and configuration.
   */
  protected function lookupCommandClass($op, $class, $config) {
    $command_info = $this->getCommandInfo();
    if (!empty($command_info[$op]['handler'])) {
      $class = $command_info[$op]['handler'];
    }
    if (!empty($command_info[$op]['handler config'])) {
      $config = $command_info[$op]['handler config'];
    }

    return array($class, $config);
  }
}
