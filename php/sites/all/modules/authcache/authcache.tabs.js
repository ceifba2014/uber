(function ($) {
  Drupal.behaviors.authcacheTabs = {
    attach: function (context, settings) {
      // Dynamically theme local task tab items for logged-in users (nodes, etc)
      var needsUpdate =
        $('#authcache-tabs', context).once('authcache-tabs').length +
        $('#authcache-local-actions', context).once('authcache-local-actions').length;

      if (needsUpdate) {
        $.AuthcacheAjax.cmd('menu_local_tasks', function(evt, result) {
          $("#authcache-tabs", context).html(result.tabs);
          $("#authcache-local-actions", context).html(result.actionLinks);
        });
      }
    }
  };
}(jQuery));
