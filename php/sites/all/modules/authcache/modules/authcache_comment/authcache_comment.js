(function ($) {
  Drupal.behaviors.authcacheComment = {
    attach: function (context, settings) {
      var nid = settings.authcacheComment && settings.authcacheComment.nid;
      var elnew = $(".authcache-comment-new", context).once('authcache-comment');

      if (elnew.length && nid) {
        $.AuthcacheAjax.cmd('node_history', nid, function(cmd, historyTimestamp) {
          elnew.each(function() {
            timestamp = this.getAttribute("data-timestamp");

            if (timestamp >= historyTimestamp) {
              $(this).hide().html(Drupal.t("new")).fadeIn();
            }
          });
        });
      }

      var nids = [];
      var elnum = {};
      $(".authcache-comment-num-new", context).once('authcache-comment-num-new', function() {
        var nid = $(this).attr('data-node-nid');
        if (nid) {
          nids.push(nid);
          elnum[nid] = this;
          $(this).hide();
        }
      });
      if (nids.length) {
        $.AuthcacheAjax.cmd('comment_num_new', nids, function(cmd, nums) {
          $.each(elnum, function(nid, el) {
            if (nums && nums[nid] > 0) {
              $(el).html(Drupal.formatPlural(nums[nid], '1 new comment', '@count new comments'));
              $(el).show();
            }
          });
        });
      }

      $(".authcache-comment-edit", context).once('authcache-comment-edit', function() {
        var uid = $(this).attr('data-comment-uid');

        if (uid === $.cookie("drupal_uid")) {
          $(this).wrap('<a href="' + $(this).attr('data-comment-href') + '"/>');
        }
        else {
          $(this).hide();
        }
      });
    }
  };
}(jQuery));
