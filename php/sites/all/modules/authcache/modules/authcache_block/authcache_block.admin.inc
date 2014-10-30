<?php

/**
 * @file
 * Admin forms and pages.
 */

/**
 * Authcache Ajax Blocks
 */
function authcache_block_admin_blocks($form, &$form_state) {
  drupal_set_title(t('Authcache Ajax Blocks'));

  if (!variable_get('block_cache', FALSE)) {
    drupal_set_message(t('Drupal core block cache is currently disabled. It is recommened you <a href="@performance_link">enable it</a>.', array('@performance_link' => url('admin/config/development/performance'))), 'warning');
  }

  $form['#suffix'] = '';
  $form['#prefix'] = '<p>' .
    t('!blocks may be configured to load during the Authcache Ajax phase. This is useful for dynamic or user-centric content.', array('!blocks' => l(t('Blocks'), 'admin/structure/block'))) .
    '</p><p>';
    t('There is a performance disadvantage, however, as uncached blocks require a full Drupal bootstrap by loading all Drupal files and modules, increasing server load. To improve performance, blocks may be cached server-side (<a href="@performance_link">@is_enabled</a>) and locally on the user\'s browser by using "Minimum cache lifetime" on the block configuration page. This option also prevents jumpiness when navigating across pages.', array('@performance_link' => url('admin/config/development/performance'), '@is_enabled' => variable_get('block_cache', FALSE) ? t('currently enabled') : t('currently disabled')))
    . '</p>';
  $blocks = array();
  $q = db_query("SELECT module,delta,region,title FROM {block} ORDER BY region, weight, module");
  foreach ($q as $r) {
    $blocks["$r->module-$r->delta"] = $r; //TODO: slg - is this still right in D7
  }

  $header = array(t('Region'), t('Block'), t('Cache Lifetime'));
  $rows = array();

  if ($authcache_block_blocks = variable_get('authcache_block_blocks', array())) {
    foreach ($authcache_block_blocks as $block_id => $lifetime) {
      if (isset($blocks[$block_id])) {
        $block = $blocks[$block_id];

        if (!$block->title) {
          $info = module_invoke($block->module, 'block', 'list');
          if (isset($info[$block->delta])) {
            $block->title = $info[$block->delta]['info'];
          }
        }

        if (!$block->region) {
          $block->region = '<em>' . t('disabled') . '</em>';
        }

        $rows[] = array(
          $block->region,
          l($block->title, "admin/structure/block/configure/$block->module/$block->delta", array('query' => array('destination' => 'admin/config/system/authcache/blocks'))),
          $lifetime . ' ' . t('seconds'),
        );

      }
    }
    $form['#suffix'] = theme('table', array(
      'header' => $header,
      'rows' => $rows,
    ));
  }

  $form['#suffix'] .= '<em>' . t('To add an Authcache Ajax block, visit the !blocks page and click "configure" next to the block.', array('!blocks' => l(t('Blocks'), 'admin/structure/block'))) . '</em>';
  return $form;
}
