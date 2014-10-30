<?php

class AuthcacheForumTopicNewCommand extends AuthcacheAjaxCommandHandlerBase {
  /**
   * Number of new forum topics for user
   * @see forum.module
   */
  function run($op, $vars) {
    global $user;
    $new = array();

    include_once './includes/common.inc';
    include_once './includes/path.inc';
    include_once './modules/field/field.module';
    include_once './modules/node/node.module';  // Need NODE_NEW_LIMIT definition
    include_once './modules/forum/forum.module';
    include_once './modules/filter/filter.module'; // XSS filter for l()

    foreach ($vars as $tid) {
      $new_topics = (int) _forum_topics_unread($tid, $user->uid);
      if ($new_topics) {
        $new[$tid] = l(format_plural($new_topics, '1 new', '@count new'), "forum/$tid", array('fragment' => 'new'));
      }
    }
    return $new;
  }
}
