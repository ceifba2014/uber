/**
 * @file
 * The main javascript file for the splash_offer module
 *
 * @ingroup splash_offer
 * @{
 */

(function ($) {

  Drupal.splashOffer = Drupal.splashOffer || {};

  /**
   * Set cookies as needed and defined by settings of this offer
   */
  Drupal.splashOffer.cookieHandler = function(c_value) {
    var c_name = 'splash_offer:' + Drupal.settings.splashOffer.id;
    var expiry = new Date();
    var time = expiry.getTime();

    // Set a cookie if user asks us to
    if (Drupal.settings.splashOffer.hasOwnProperty('cookie')
        && $('#splash-offer-set-cookie:checked').length) {
      time += Drupal.settings.splashOffer.cookie.expiry;
      expiry.setTime(time);
      c_value = escape(c_value) + ((expiry === null) ? "" : "; expires=" + expiry.toGMTString());
      document.cookie = c_name + "=" + c_value;
    }

    // Make sure we remove any cookies if unchecked so user will be prompted
    // again.
    if (!$('#splash-offer-set-cookie:checked').length) {
      time -= 1;
      expiry.setTime(time);
      document.cookie = c_name + "=; expires=" + expiry.toGMTString();
    }
  }

  /**
   * Show the splash offer modal
   */
  Drupal.splashOffer.show = function() {
    var options = {
      dialogClass: 'splash-offer-dialog splash-offer-dialog-' + Drupal.settings.splashOffer.id,
      height: $('.entity-splash-offer').height(),
      width: $('.entity-splash-offer').width(),
      autoOpen: true,
      modal: true,
      draggable: false,
      resizable: false,
    }
    if (Drupal.settings.splashOffer.hasOwnProperty('yes')) {
      $.extend(options, {
      buttons: [
        {
          text: Drupal.settings.splashOffer.yes.text,
          click: function() {
            Drupal.splashOffer.cookieHandler('yes');
            Drupal.splashOffer.hide();
            return Drupal.splashOffer.redirect('yes');
          }
        },
        {
          text: Drupal.settings.splashOffer.no.text,
          click: function() {
            Drupal.splashOffer.cookieHandler('no');
            Drupal.splashOffer.hide();
            return false;
          }
        }
      ]
      });
    }
    $('.entity-splash-offer').dialog(options);
  }

  /**
   * Hide the splash offer modal
   */
  Drupal.splashOffer.hide = function() {
    $('.entity-splash-offer').dialog('close');
  }

  /**
   * Redirect based on button click
   */
  Drupal.splashOffer.redirect = function(buttonId) {
    if (typeof Drupal.settings.splashOffer[buttonId].path !== 'undefined') {
      window.open(Drupal.settings.splashOffer[buttonId].path);
      return false;
    }
    return true;
  }

  /**
  * Core behavior for splash_offer.
  */
  Drupal.behaviors.splashOffer = Drupal.behaviors.splashOffer || {};
  Drupal.behaviors.splashOffer.attach = function (context, settings) {

    /**
     * Admin Entity form
     */
    $('.form-item-data-devices-always-trigger .form-checkbox').click(function() {
      // Show the devices when 'always trigger' is disabled
      var expand = !$(this).is(':checked')
      $(this).parents('.fieldset-wrapper').find('fieldset').each(function() {
        if ((expand && $(this).hasClass('collapsed'))
            || (!expand && !$(this).hasClass('collapsed'))) {
          $(this).find('a.fieldset-title').click();
        }
      });
    });

    /**
     * Displaying a splash offer
     *
     * We are displaying a splash offer if we have an oid
     */
    if (typeof Drupal.settings.splashOffer !== 'undefined'
        && Drupal.settings.splashOffer.hasOwnProperty('id')) {
      Drupal.splashOffer.show();

      $('.splash-offer-no').click(function(){
        Drupal.splashOffer.cookieHandler('no');
        Drupal.splashOffer.hide();
        return false;
      });

      $('.splash-offer-yes').click(function(){
        Drupal.splashOffer.cookieHandler('yes');
        Drupal.splashOffer.hide();
        return Drupal.splashOffer.redirect('yes');
      });
    }


  }

  /**
  * @} End of "defgroup splash_offer".
  */

})(jQuery);
