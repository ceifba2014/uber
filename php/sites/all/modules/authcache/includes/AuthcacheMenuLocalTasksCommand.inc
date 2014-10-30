<?php

/**
 * Render primary & secondary tabs.
 * Response will be cached.
 * @see menu.inc
 */
class AuthcacheMenuLocalTasksCommand extends AuthcacheAjaxCommandHandlerBase {
  function run($op, $args) {
    return array(
      'tabs' => render(menu_local_tabs()),
      'actionLinks' => render(menu_local_actions()),
    );
  }
}
