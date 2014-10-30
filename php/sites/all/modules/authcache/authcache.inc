<?php

/**
 * @file
 * Attempt to deliver a cached version of a page depending on the users role.
 *
 * This file gets included by _drupal_bootstrap_page_cache in bootstrap.inc
 * during the drupal bootstrap stage DRUPAL_BOOTSTRAP_PAGE_CACHE. This script
 * delegates the request to the underlying cache handler and attempts to deliver
 * a cached version of a page for this request.
 *
 * In the event of a cache-miss or if a page is not cachable, execution is
 * passed back to _drupal_bootstrap_page_cache and the page will be served
 * using a full bootstrap.
 *
 * @see _drupal_bootstrap_page_cache in boostrap.inc
 */

// Attempt to deliver the page from cache
if (variable_get('authcache_cache_handler', 'authcache_builtin_cache_save') == 'authcache_builtin_cache_save') {
  $delivered = authcacheinc_retrieve_cache_page();
  if ($delivered) {
    exit;
  }
}

/**
 * Send cached page to browser, if found.
 *
 * @return boolean
 *   TRUE if page was delivered, FALSE otherwise
 */
function authcacheinc_retrieve_cache_page(){
  global $base_root;

  // The following three basic exclusion rules are mirrored in
  // authcache_authcache_request_exclude() in authcache.module
  // BEGIN: basic exclusion rules
  if (drupal_is_cli()) {
    return FALSE;
  }

  if (!($_SERVER['REQUEST_METHOD'] == 'GET' || $_SERVER['REQUEST_METHOD'] == 'HEAD')) {
    return FALSE;
  }

  if (($ar = explode('?', basename(request_uri()))) && substr(array_shift($ar), -4) == '.php') {
    return FALSE;
  }
  // END: basic exclusion rules

  // Figure out the authcache key
  $has_cookie = isset($_COOKIE['authcache']);
  $has_session = isset($_COOKIE[session_name()]);

  if ($has_cookie && $has_session) {
    // Cookie and session present, deliver from cache
    $key = $_COOKIE['authcache'];
  }
  else if(!($has_cookie || $has_session)) {
    // Neither cookie nor session present, deliver from cache for anonymous
    // users.
    $generator = variable_get('authcache_key_generator');
    if (is_callable($generator)) {
      $key = call_user_func($generator);
    }
    else {
      $key = $base_root;
    }
  }
  else {
    // Weird. Need to bootstrap fully and figure out whats going on later.
    return FALSE;
  }

  // Connect to database if default database cache hander is selected
  if (get_class(_cache_get_object('cache_page')) == 'DrupalDatabaseCache') {
    drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE, FALSE);
  }

  // Attempt to retrieve page from cache
  $cid = $key . request_uri();
  $cache = cache_get($cid, 'cache_page');

  if (empty($cache)) {
    header('X-Drupal-Cache: MISS');
    return FALSE;
  }
  else {
    // render cache benchmark
    if (isset($_COOKIE['cache_render'])) {
      setcookie('cache_render', timer_read('page'), 0, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
    }

    header('X-Drupal-Cache: HIT');
    drupal_serve_page_from_cache($cache);
    return TRUE;
  }
}
