<?php

/**
 * Exclude a page from being cached based on the request.
 *
 * @return A transalted string specifying the reason of exclusion or null.
 */
function hook_authcache_request_exclude() {
  if (authcache_ajax_is_authcache_ajax_request()) {
    return t('Authcache Ajax request');
  }
}

/**
 * Exclude a page from being cached basde on the given account.
 *
 * @return A translated string specifying the reason of exclusion or null.
 */
function hook_authcache_account_exclude($account) {
  // Bail out from requests by superuser (uid=1)
  if ($account->uid == 1 && !variable_get('authcache_su', 0)) {
    return t('Caching disabled for superuser');
  }
}

/**
 * Perform an action when a page has been excluded from caching.
 *
 * This hook is called very early in authcache_init().
 *
 * @param $reason
 *   A translated string giving the reason why the page was excluded from being
 *   cached.
 *
 * @see hook_authcache_request_exclude().
 * @see hook_authcache_account_exclude().
 */
function hook_authcache_excluded($reason) {
  if (authcache_debug_access()) {
    drupal_add_js(array('authcacheDebug' => array('nocacheReason' => $reason)), 'setting');
  }
}

/**
 * Perform an action when page caching has been cancelled.
 *
 * This hook may be called very late, i.e. after the page was built und just
 * before it is sent to the browser.
 *
 * @param $reason
 *   A translated string giving the reason why page caching has been cancelled.
 *
 * @see authcache_cancel().
 */
function hook_authcache_cancelled($reason) {
  if (authcache_debug_access()) {
    setcookie('nocache_reason', $reason, 0, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
  }
}

/**
 * Return characterizing facts on the users browser or the HTTP request.
 *
 * @return An associative array of key-value pairs.
 *
 * @see authcache_get_request_property().
 */
function hook_authcache_request_properties() {
  return array(
    'js' => !empty($_COOKIE['has_js']),
  );
}

/**
 * Modify characterizing facts on the users browser or the HTTP request defined
 * by another module.
 *
 * @see hook_authcache_request_properties().
 */
function hook_authcache_request_properties_alter(&$properties) {
  // FIXME: Do we really have a usecase for that? Probably this hook should be
  // removed.
}

/**
 * Return characterizing facts on the given account.
 *
 * @return An associative array of key-value pairs.
 *
 * @see authcache_get_account_property().
 */
function hook_authcache_account_properties($account) {
  $roles = array_keys($account->roles);
  sort($roles);

  return array(
    'roles' => $roles,
  );
}

/**
 * Modify characterizing facts on the given account defined by another module.
 *
 * @see hook_authcache_account_properties().
 */
function hook_authcache_account_properties_alter(&$properties, $account) {
  // FIXME: Do we really have a usecase for that? Probably this hook should be
  // removed.
}

/**
 * Modify the properties used to calculate the authcache key.
 *
 * @see authcache_key_properties().
 * @see authcache_key().
 */
function hook_authcache_key_properties_alter(&$properties, $account) {
  // Paranoia-mode: Make sure the authcache-key for logged in users changes
  // every hour.
  if ($account->uid) {
    $properties['timeslice'] = floor(REQUEST_TIME/3600);
  }
}

/**
 * Return information about cookies in use.
 *
 * Modules and themes may declare the characteristics of cookies they use by
 * implementing this hook. Doing so will allow authcache to manage those
 * cookies, i.e. setting and deleting them when a user-session is started and
 * terminated respectively.
 *
 * @param $flags
 *   A bitmask of flags describing the action which triggered this hook
 *   invocation as well as the account is enabled for authcache. The following
 *   are used:
 *   - AUTHCACHE_FLAGS_ACCOUNT_ENABLED: Caching is allowed for the given
 *     account.
 *   - AUTHCACHE_FLAGS_LOGIN_ACTION: Hook was invoked as a result of a user
 *     logging in.
 *   - AUTHCACHE_FLAGS_LOGOUT_ACTION: Hook was invoked as a result of a user
 *     logging out.
 * @param $account
 *   The user object on which the operation was just performed.
 *
 * @return
 *   An array of cookie items. Each cookie item has a key corresponding
 *   to the cookie-name. The corresponding array value is an associative array
 *   that may contain the following key-value pairs:
 *   - "present": TRUE if the cookie should be present in the users browser,
 *     FALSE otherwise. Defaults to FALSE.
 *   - "value": The cookies value. Defaults to NULL.
 *   - "lifetime": An integer value specifying how many seconds the cookie
 *     should be kept by the browser. Defaults to the PHP ini value
 *     session.cookie_lifetime.
 *   - "path": The path in which the cookie will be available on. Defaults to
 *     the PHP ini value session.cookie_path.
 *   - "domain": The domain that the cookie is available to. Defaults to the
 *     PHP ini value session.cookie_domain.
 *   - "secure": Indicates that the cookie should only be transmitted over a
 *     secure HTTPS connection from the client. Defaults to the PHP in value
 *     session.cookie_secure.
 *   - "httponly": When TRUE the cookie will be made accessible only through
 *     the HTTP protocol. This means that the cookie won't be accessible by
 *     scripting languages, such as JavaScript. Defaults to FALSE.
 *
 * @see authcache_fix_cookies().
 * @see setcookie().
 */
function hook_authcache_cookie($flags, $account) {
  $authenticated = $account->uid;
  $enabled = $flags & AUTHCACHE_FLAGS_ACCOUNT_ENABLED;
  $present = $authenticated && $enabled;

  $cookies['aceuser']['present'] = $present;

  if ($present) {
    $cookies['aceuser']['value'] =  $account->name;
  }

  return $cookies;
}

/**
 * Modify information about cookies given by other modules.
 *
 * @see hook_authcache_cookie().
 * @see authcache_fix_cookies().
 */
function hook_authcache_cookie_alter(&$cookies, $flags, $account) {
}

/**
 * Perform last-minute checks before a built page is saved to the cache.
 *
 * @see authcache_page_set_cache().
 */
function hook_authcache_presave() {
  // Make sure "Location" redirect isn't used
  foreach (headers_list() as $header) {
    if (strpos($header, 'Location:') === 0) {
      authcache_cancel(t('Location header detected'));
    }
  }
}
