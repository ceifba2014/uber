(function ($) {
  Drupal.behaviors.authcacheFormTokenIds = {
    attach: function (context, settings) {
      if ($.isEmptyObject(settings.authcacheFormTokenIds)) {
        return;
      }

      // Retrieve tokens using an AJAX command.
      $.AuthcacheAjax.cmd('form_token_id', settings.authcacheFormTokenIds, function(evt, result) {
        if ($.isEmptyObject(result)) {
          return;
        }

        // Inject the hidden form_token input after each hidden form_id input.
        $("form input[name='form_id']", context).once('authcacheFormTokenIds').after(function() {
          if (settings.authcacheFormTokenIds[this.value] && result[this.value]) {
            return "<input type='hidden' name='form_token' value='" + result[this.value] + "'/>"
          }
        });
      });
    }
  };

  Drupal.behaviors.authcacheFormBuildId = {
    attach: function (context, settings) {
      if ($.isEmptyObject(settings.authcacheFormTokenIds)) {
        return;
      }

      // Retrieve new build id using an AJAX command.
      $.AuthcacheAjax.cmd('form_build_id', settings.authcacheFormBuildIds, function(evt, result) {
        if ($.isEmptyObject(result)) {
          return;
        }

        // Inject the hidden form_build_id input after each hidden form_id input.
        $("form input[name='form_id']", context).once('authcacheFormBuildIds').after(function() {
          if (settings.authcacheFormBuildIds[this.value] && result[this.value]) {
            return "<input type='hidden' name='form_build_id' value='" + result[this.value] + "'/>"
          }
        });
      });
    }
  };
}(jQuery));
