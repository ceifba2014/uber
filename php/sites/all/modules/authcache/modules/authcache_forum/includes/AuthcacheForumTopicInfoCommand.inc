<?php

class AuthcacheForumTopcInfoCommand extends AuthcacheAjaxCommandHandlerBase {
  /**
   * Number of new topic replies for user or topic is unread
   * @see forum.module
   */
  function run($op, $vars) {
    global $user;
    $info = array();

    include_once './includes/common.inc';
    include_once './includes/path.inc';
    include_once './modules/field/field.module';
    include_once './modules/node/node.module';  // Need NODE_NEW_LIMIT definition
    include_once './modules/forum/forum.module';
    include_once './modules/comment/comment.module';

    foreach ($vars as $rec) {
      $nid = $rec['nid'];
      $timestamp = $rec['timestamp'];

      $history = _forum_user_last_visit($nid);
      $new_topics = (int)comment_num_new($nid, $history);
      if ($new_topics) {
        $info[$nid] = format_plural($new_topics, '1 new', '@count new');
      }
      elseif ($timestamp > $history) { // unread
        $info[$nid] = 1;
      }
    }

    return $info;
  }
}
