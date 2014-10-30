(function ($) {
  Drupal.behaviors.authcacheForum = {
    attach: function (context, settings) {
      var newtopics = [];
      var topicinfo = [];

      $(".authcache-topic-new").once('authcache-topic', function() {
        if ($.cookie('drupal_uid')) {
          newtopics.push($(this).attr('data-forum-id'));
        }
      });

      if (newtopics.length) {
        $.AuthcacheAjax.cmd('forum_topic_new', newtopics, function(evt, result) {
          $.each(result || [], function(id, content) {
            $(".authcache-topic-new[data-forum-id=" + id + "]").before("<br />").hide().html(content).fadeIn();
          });
        });
      }

      $('.authcache-topic-info').each(function() {
        if ($.cookie('drupal_uid')) {
          var ts = $(this).attr("data-timestamp");
          var nid = $(this).attr("data-nid");
          topicinfo.push({'nid': nid, 'timestamp': ts});
        }
      });

      if (topicinfo.length) {
        $.AuthcacheAjax.cmd('forum_topic_info', topicinfo, function(evt, result) {
          $.each(result || [], function(id, content) {
            $(".authcache-topic-replies[data-nid=" + id + "]").before("<br />").hide().html(content).fadeIn();
            var oIcon = $(".authcache-topic-icon[data-nid=" + id + "]");
            oIcon.html(oIcon.html().replace(/default/g, "new"));
            oIcon.html(oIcon.html().replace(/-hot/g, "-hot-new"));
          });
        });
      }
    }
  };
}(jQuery));
