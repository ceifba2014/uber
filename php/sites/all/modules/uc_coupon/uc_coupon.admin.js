(function($) {
  Drupal.behaviors.ucCouponAdmin = {
    attach: function(context) {
      function toggleUsageLimits() {
        if ($('#edit-store-credit').is(':checked')) {
          $('#edit-usage-limits').hide();
        }
        else {
          $('#edit-usage-limits').show();
        }
      }
      $('#edit-store-credit', context).click(toggleUsageLimits);

      $('#edit-discount', context).keyup(function() {
        if (this.value.indexOf('%') == -1) {
          $(this).siblings('span').show();
        }
        else {
          $(this).siblings('span').hide();
        }
        if (this.value.indexOf('%') != -1 || this.value.indexOf('=') != -1) {
          $('#store-credit-wrapper').hide();
          $('#edit-usage-limits').show();
        }
        else {
          $('#store-credit-wrapper').show();
          toggleUsageLimits();
        }
      }).keyup();

      $('#edit-store-credit', context).click(function() {
        if ($(this).is(':checked')) {
          $('#edit-usage-limits').hide();
        }
        else {
          $('#edit-usage-limits').show();
        }
      });
    
      $('input[name=apply_to]', context).click(function() {
        if (this.value == 'cheapest' || this.value == 'expensive') {
          $('.form-item-apply-count').show();
        }
        else {
          $('.form-item-apply-count').hide();
        }
      }).filter(':checked').click();
    
      if ($('input[name=use_validity]', context).change(function() {
        $('.form-item-valid-from, .form-item-valid-until').toggle();
      }).is(':not(:checked)')) {
        $('.form-item-valid-from, .form-item-valid-until').hide();
      }
    }
  };
})(jQuery);
