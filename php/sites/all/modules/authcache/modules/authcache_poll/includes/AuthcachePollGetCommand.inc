<?php

class AuthcachePollGetCommand extends AuthcacheAjaxCommandHandlerBase {
  /**
   * Get poll results/form for user
   * Response will be cached.
   * @see poll.module
   */
  function run($op, $vars) {
    $nodes = node_load_multiple($vars);
    $build = node_view_multiple($nodes);

    $result = array();

    foreach (element_children($build['nodes']) as $nid) {
      if (isset($build['nodes'][$nid]['poll_view_voting'])) {
        $output = render($build['nodes'][$nid]['poll_view_voting']);
      }
      elseif (isset($build['nodes'][$nid]['poll_view_results'])) {
        $output = render($build['nodes'][$nid]['poll_view_results']);
      }

      $result[$nid] = $output;
    }

    return $result;
  }
}
