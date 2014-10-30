<?php
/**
 * @file
 * Defines authcache ajax command for retrieving debug information about the
 * current page.
 */

class AuthcacheDebugInfoCommand extends AuthcacheAjaxCommandHandlerBase {

  /**
   * Return debug information for the given page request.
   */
  function run($op, $key) {
    include_once dirname(__FILE__) . '/../authcache_debug.module';

    if (authcache_debug_access()) {
      $cache = cache_get($key, 'cache_authcache_debug');

      if ($cache) {
        return $cache->data;
      }
    }
  }
}
