<?php
/**
 * @file
 * Defines an authcache ajax callback for maintaining the last-viewed timestamp.
 */

/**
 * Authcache ajax callback for retrieving the number of new comments given a
 * list of nids.
 *
 * @see node.module
 */
class AuthcacheCommentNumNewCommand extends AuthcacheAjaxCommandHandlerBase {

  /**
   * Number of new comments for given nids.
   */
  function run($op, $nids) {
    include_once './modules/node/node.module';
    include_once './modules/comment/comment.module';

    $counts = array_map('comment_num_new', $nids);

    return array_combine($nids, $counts);
  }
}
