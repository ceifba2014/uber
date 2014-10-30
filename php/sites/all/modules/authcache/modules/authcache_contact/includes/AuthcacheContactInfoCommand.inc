<?php

class AuthcacheContactInfoCommand extends AuthcacheAjaxCommandHandlerBase {
  /**
   * Return default form values for site contact form
   * @see contact.module
   */
  function run($op, $vars) {
    global $user;
    return array('name' => $user->name, 'mail' => $user->mail);
  }
}
