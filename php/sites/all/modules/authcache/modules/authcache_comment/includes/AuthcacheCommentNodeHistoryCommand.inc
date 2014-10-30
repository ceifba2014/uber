<?php
/**
 * @file
 * Defines an authcache ajax callback for maintaining the last-viewed timestamp.
 */

/**
 * Authcache ajax callback for maintaining and retrieving the per-user timestamp 
 * when a given node has last been visited. This information is then used by 
 * the frontend script to mark new comments.
 *
 * @see node.module
 */
class AuthcacheCommentNodeHistoryCommand extends AuthcacheAjaxCommandHandlerBase {

  /**
   * Node history
   */
  function run($op, $nid) {
    global $user;

    include_once './modules/node/node.module';

    // After a page was cleaned from the cache, the first user requesting it will
    // trigger a node_tag_new and update its last viewed timestamp. Therefore we
    // store the timestamp of the last visit during page-build in the session and
    // retrieve it here.
    if (!empty($_SESSION['authcache_comment_lastviewed']) && $_SESSION['authcache_comment_lastviewed']['nid'] == $nid) {
      $timestamp = $_SESSION['authcache_comment_lastviewed']['timestamp'];
    }
    else {
      // Retrieves the timestamp at which the current user last viewed the specified node
      $timestamp = node_last_viewed($nid);

      // Update the 'last viewed' timestamp of the specified node for current user.
      // We do not want to use node_tag_view here because it requires a node_load
      // which seems impossible when drupal is not fully bootstraped. The following
      // code is directly copied from node_tag_view.
      if ($user->uid) {
        db_merge('history')
          ->key(array(
            'uid' => $user->uid,
            'nid' => $nid,
          ))
          ->fields(array('timestamp' => REQUEST_TIME))
          ->execute();
      }
    }

    unset($_SESSION['authcache_comment_lastviewed']);

    return $timestamp;
  }
}
