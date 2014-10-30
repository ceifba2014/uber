(function ($) {
  Drupal.behaviors.authcacheBlock = {
    attach: function (context, settings) {
      var cmds = {};

      // Authcache blocks. Create one request for each block having a specified
      // maxAge parameter.
      $('.authcache-block', context).once('authcache-block', function() {
        var cid = $(this).attr('data-block-cid');
        var id = $(this).attr('data-block-id');
        var key = $(this).attr('data-block-req');

        var req = $.AuthcacheAjax.request(key);
        var cmd = req.command('blocks', {});

        cmd.args[id] = cid;

        cmds[key] = cmd;
      });

      $.each(cmds, function() {
        $(this).bind('result', function(evt, result) {
          $.each(result, function(id, block) {
            $("#authcache-block-subj-" + id).html(block.subject);
            $("#authcache-block-" + id).html(block.content);
          });
        });
      });
    }

  };
}(jQuery));
