<?php
/**
 * @file
 * Defines command handler for ajax-blocks.
 */


/**
 * Command handler for ajax-blocks.
 *
 * FIXME: Due to the fact that blocks are cached as render-array on the server,
 * we need a full bootstrap plus initialized theming layer. Perhaps we find a
 * better way to reduce the bootstrap level by maintaining a authcache-only 
 * version of the block-cache.
 */
class AuthcacheBlockGetCommand extends AuthcacheAjaxCommandHandlerBase {
  /**
   * Render blocks. Grab from cache if available.
   * @param <array> $blocks
   *   [block id] => [block cache id]
   * @see block.module
   */
  function run($op, $blocks) {
    drupal_theme_initialize();

    global $user, $theme_key;
    $return = array();

    foreach ($blocks as $block_id => $block_cid) {
      // If block cache is per user, then specify current user id.
      $block_cid = preg_replace('/:u.[0-9]+/', ":u.$user->uid", $block_cid);

      // Validate user roles with block visibility roles.
      // (In case someone is trying to hack into viewing certain blocks.)
      if (strpos($block_cid, ':r.') !== FALSE) {
        $matches = array();
        preg_match('/:r.([0-9,]+)/', $block_cid, $matches);
        if (isset($matches[1])) {
          // Cache id is built using exact user roles, so a direct comparison works. @see _block_get_cache_id().
          if ($matches[1] != implode(',', array_keys($user->roles))) {
            continue;
          }
        }
      }

      // Check cache_block bin first
      if ($block_cached = cache_get($block_cid, 'cache_block')) {
        $return[$block_id] = array(
          'subject' => check_plain($block_cached->data['subject']),
          'content' => render($block_cached->data['content']),
        );
      }
      else {
        $id = explode('-', $block_id, 2);

        $block = block_load($id[0], $id[1]);
        $build = reset(_block_get_renderable_array(_block_render_blocks(array($block_id => $block))));
        if (!empty($build['#theme_wrappers'])) {
          $build['#theme_wrappers'] = array_diff($build['#theme_wrappers'], array('block'));
        }

        $return[$block_id] = array(
          'subject' => check_plain($build['#block']->subject),
          'content' => render($build),
        );
      }
    }

    return $return;
  }
}
