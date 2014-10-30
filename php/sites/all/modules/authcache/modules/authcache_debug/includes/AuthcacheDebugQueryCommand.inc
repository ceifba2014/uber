<?php
/**
 * @file
 * Defines authcache ajax handler capable of retrieving executed database
 * queries for a given page.
 */

/**
 * Database benchmarks for Authcache Ajax phase
 */
class AuthcacheDebugQueryCommand extends AuthcacheAjaxCommandHandlerBase {
  function run($op, $args) {
    global $queries;
    if (!$queries) return;

    $time_query = 0;
    foreach ($queries as $q) {
      $time_query += $q[1];
    }
    $time_query = round($time_query * 1000, 2); // Convert seconds to milliseconds
    $percent_query = round(($time_query / timer_read('page')) * 100);

    return count($queries) . " queries @ {$time_query} ms";
  }
}
