<?php

/**
 * Generate new per-user form-cache entries by copying them from prototypes.
 *
 * @see form.inc
 */
class AuthcacheFormBuildIdCommand extends AuthcacheAjaxCommandHandlerBase {
  public function run($op, $forms) {
    include_once DRUPAL_ROOT . '/includes/common.inc';
    include_once DRUPAL_ROOT . '/includes/form.inc';

    foreach ($forms as $form_id => $form_build_id) {
      // Retrieve prototype from form cache. Authcache prototype forms are
      // prefixed with "form_authcache-".
      if ($cached = cache_get('form_authcache-' . $form_build_id, 'cache_form')) {
        $form = $cached->data;

        global $user;
        if ($form['#form_id'] == $form_id) {
          if ($cached = cache_get('form_state_authcache-' . $form_build_id, 'cache_form')) {
            $form_state = $cached->data;

            $new_build_id = 'form-' . drupal_hash_base64(uniqid(mt_rand(), TRUE) . mt_rand());
            $form['#build_id'] = $new_build_id;
            $form['form_build_id']['#value'] = $new_build_id;
            $form['form_build_id']['#id'] = $new_build_id;

            // Store new form for the current user in the form cache. Note that
            // form_set_cache will replace the per-user #cache_token before
            // storing it.
            form_set_cache($new_build_id, $form, $form_state);

            // Report the new build-id back to the browser
            $buildids[$form_id] = $new_build_id;
          }
        }
      }
    }

    return $buildids;
  }
}
