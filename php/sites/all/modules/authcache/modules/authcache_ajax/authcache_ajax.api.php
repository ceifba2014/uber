<?php

/**
 * Define ajax request-type for this module.
 * 
 * @return
 *   An associative array whose keys are group names and whose values are an
 *   associative array containing:
 *   - 'class': (optional) Name of a class implementing AuthcacheAjaxHandlerFactory. 
 *     Defaults to AuthcacheAjaxDefaultRequestHandler.
 *   - 'maxage': (optional) Seconds the results should be cached in the browser 
 *     when client-side caching of requests by this type is desired.
 *   - 'cookie': (optional) The cookie name used for browser-side invalidation
 *     control. Defaults to _authcache_<request-type>_vers.
 */
function hook_authcache_ajax_request() {
  $request['poll'] = array(
    'maxage' => 600,
  );
  return $request;
}

/**
 * Modify ajax request-types.
 *
 * @see hook_authcache_ajax_request().
 */
function hook_authcache_ajax_request_alter(&$requests) {
  if (isset($requests['poll'])) {
    // Use cookie with name authcache_pollvers instead of _authcache_poll_vers
    $requests['poll']['cookie'] = 'acpollvers';
  }
}

/**
 * Define ajax commands for this module.
 * 
 * @return
 *   An associative array whose keys are command names names and whose values
 *   are an associative array containing:
 *   - 'class': Name of a class implementing AuthcacheAjaxCommandHandler.
 *   - 'bootstrap': (optional) If class inherits from AuthcacheAjaxCommandHandlerBase, 
 *     a bootstrap level the command requires in order to be executed properly.
 */
function hook_authcache_ajax_command() {
  return array(
    'poll' => array(
      'class' => 'AuthcachePollGetCommand',
      // FULL bootstrap required in case custom theming is used
      'bootstrap' => DRUPAL_BOOTSTRAP_FULL,
    ),
  );
}

/**
 * Modify command definitions.
 *
 * @see hook_authcache_ajax_command().
 */
function hook_authcache_ajax_command_alter(&$commands) {
}
