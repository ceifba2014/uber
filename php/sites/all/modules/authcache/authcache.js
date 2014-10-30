(function($) {
  $.authcache_cookie = function(name, value, lifetime) {
    lifetime = (typeof lifetime === 'undefined') ? Drupal.settings.authcache.cl : lifetime;
    $.cookie(name, value, $.extend(Drupal.settings.authcache.cp, {expires: lifetime}));
  };

  Drupal.behaviors.authcache = {
    attach: function (context, settings) {
      if ($.cookie('drupal_user')) {
        // Display logged-in username
        $('.authcache-user', context).once('authcache-user').html($.cookie('drupal_user'));
      }
    }
  }
}(jQuery));
