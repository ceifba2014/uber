(function ($) {
  Drupal.behaviors.authcachePoll = {
    attach: function (context, settings) {
      // Get poll results/form
      var nids = [];
      $(".authcache-poll", context).each(function() {
        nids.push($(this).attr('data-nid'));
      });

      if (nids.length) {
        $.AuthcacheAjax.cmd('poll', nids, function(evt, result) {
          $.each(result, function(nid, content) {
            Drupal.attachBehaviors($('.authcache-poll[data-nid="' + nid + '"]').html(content));
          });
        });
      }
    }
  };
}(jQuery));
