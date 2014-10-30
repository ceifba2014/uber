(function ($) {
  $(function() {
    if (Drupal.settings.authcacheStatistics && Drupal.settings.authcacheStatistics.log) {
      $.AuthcacheAjax.cmd('statistics', Drupal.settings.authcacheStatistics);
    }
  });
}(jQuery));
