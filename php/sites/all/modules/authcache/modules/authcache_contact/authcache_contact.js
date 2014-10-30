(function ($) {
  Drupal.behaviors.authcacheContact = {
    attach: function (context, settings) {
      $('#contact-site-form', context).once(function() {
        var $form = $(this);
        $.AuthcacheAjax.cmd('contact', function(evt, result) {
          $form.find("input[name='name']").val(result.name);
          $form.find("input[name='mail']").val(result.mail);
        });
      });
    }
  };
}(jQuery));
