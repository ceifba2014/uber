<?php

/**
 * @file
 * Authcache Ajax Callback (authcache.php)
 *
 * The Authcache Ajax phase, included by ../authcache.inc during the drupal
 * bootstrap stage DRUPAL_BOOTSTRAP_PAGE_CACHE.
 *
 * Calls functions as defined in GET request: _authcache_{key} => value(s)
 * (Uses Authcache:ajax JSON from authcache.js)
 * Outputs JSON object of values returned by functions, if any.
 *
 * DO NOT MODIFY THIS FILE!
 * Place custom functions into sites/yoursite/authcache_custom.php. Additionally
 * you may place functions into authcache_custom.php in the same directory as
 * this file.
 *************************************************************/

// Attempt to deliver an ajax response, otherwise just continue with the normal 
// request-response cycle.
$delivered = authcache_ajax_retrieve_ajax_response();
if ($delivered) {
  exit;
}

function authcache_ajax_retrieve_ajax_response() {
  global $user;

  if (!isset($_SERVER['HTTP_AUTHCACHE'])) {
    return FALSE;
  }

  $REQ_DEFAULTS = array('g' => NULL, 'v' => NULL);

  // GET is faster than POST, but has a character limit and less secure (easier to log)
  $REQ = (($_SERVER['REQUEST_METHOD'] == 'POST') ? $_POST : $_GET) + $REQ_DEFAULTS;

  // Set current page for bootstrap
  if (isset($_POST['q'])) {
    $_GET['q'] = $_POST['q'];
  }

  // Continue Drupal bootstrap. Establish database connection and validate session.
  drupal_bootstrap(DRUPAL_BOOTSTRAP_SESSION, TRUE);

  // If user session is invalid/expired run a full bootstrap. This will result
  // in the invocation of hook_init and consequently will give authcache a
  // chance to fix cookies.
  if (!$user->uid && isset($_COOKIE['authcache'])) {
    drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
  }

  // Shortcut: If we're loading the default factory, we skip the autoloader.
  $factoryclass = variable_get('authcache_ajax_factory', 'AuthcacheAjaxDefaultHandlerFactory');
  if ($factoryclass === 'AuthcacheAjaxDefaultFactory') {
    include_once dirname(__FILE__) . '/includes/AuthcacheAjaxCommandHandler.inc';
    include_once dirname(__FILE__) . '/includes/AuthcacheAjaxHandlerFactory.inc';
    include_once dirname(__FILE__) . '/includes/AuthcacheAjaxRequestHandler.inc';
    include_once dirname(__FILE__) . '/includes/AuthcacheAjaxDefaultHandlerFactory.inc';
    include_once dirname(__FILE__) . '/includes/AuthcacheAjaxDefaultRequestHandler.inc';
  }

  // Construct factory class.
  $factory = new $factoryclass();

  // Setup request
  $req = $factory->getRequestHandler($REQ);
  $max_age = $req->getCacheMaxAge();

  // Run commands
  $result = array();
  foreach ($req->getCommands() as $op => $params) {
    $cmd = $factory->getCommandHandler($op);
    if ($cmd) {
      $r = $cmd->run($op, $params);
      if ($r !== NULL) {
        $result[$op] = $r;
      }
    }
  }

  // Build response
  $response = $req->getResponse($result);

//  // Calculate database benchmarks, if enabled.
//  if (variable_get('dev_query', FALSE)) {
//    $response['db_queries'] = _authcache_dev_query();
//  }

  // Should browser cache this response?
  if ($max_age > 0) {
    // Tell browser to cache response for 'max_age' seconds
    header("Cache-Control: max-age={$max_age}, must-revalidate");
    header('Expires: ' . gmdate('D, d M Y H:i:s', REQUEST_TIME+24*60*60) . ' GMT'); // 1 day
  }

  header('Vary: Cookie');
  header("Content-type: text/javascript");

  // Extracted from drupal_json_encode in common.inc
  if (version_compare(PHP_VERSION, '5.3.0', '>=')) {
    // Encode <, >, ', &, and " using the json_encode() options parameter.
    print json_encode($response, JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_AMP | JSON_HEX_QUOT);
  }
  else {
    // json_encode() escapes <, >, ', &, and " using its options parameter, but
    // does not support this parameter prior to PHP 5.3.0.  Use a helper instead.
    include_once DRUPAL_ROOT . '/includes/json-encode.inc';
    print drupal_json_encode_helper($response);
  }

  return TRUE;
}
