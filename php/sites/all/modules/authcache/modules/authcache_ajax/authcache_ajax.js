(function ($) {
  /**
   * Construct a new private authcache request object.
   */
  function Req(url, params) {
    this.url = url;
    this.commands = {};
    this.usePost = Drupal.settings.authcacheAjax.post;
    this.async = true;
    this.params = params || {};
  }

  /**
   * Add one command object to the request. Optionally specify parameters and a
   * result handler callback.
   */
  Req.prototype.command = function(op, args, callback) {
    var cmd;

    if (typeof callback === 'undefined' && typeof args === 'function') {
      callback = args;
      args = '';
    }

    if (!(cmd = this.commands[op])) {
      cmd = {'op': op, 'args': args};
      this.commands[op] = cmd;
    }

    if (typeof callback === 'function') {
      $(cmd).bind('result', callback);
    }

    if (!this.scheduled) {
      this.scheduled = setTimeout($.proxy(this.send, this), 0);
    }

    return cmd;
  };

  /**
   * Set the value of a request-parameter.
   */
  Req.prototype.param = function(name, value) {
    this.params[name] = value;
    return this;
  };

  /**
   * Set the async flag to false
   */
  Req.prototype.sync = function() {
    this.async = false;
    return this;
  };

  /**
   * Return a list of parameters as an object.
   */
  Req.prototype.buildParams = function() {
    var params = {};

    $.each(this.commands, function(op) {
      if (typeof this.args !== undefined) {
        params[op] = this.args;
      }
    });

    this.params = $.extend(this.params, params);
  };

  /**
   * Send request and trigger result handler
   */
  Req.prototype.send = function() {
    this.buildParams();

    $.ajax({
      url: this.url,
      type: (this.usePost || $.param(this.params).length > 2000) ? "POST" : "GET",
      dataType: "json",
      data: this.params,
      context: this,

      // If response is to be cached (max_age), then a syncronous request
      // will lock the browser & prevent jumpiness on HTML DOM updates
      async: this.async,

      // Custom header to help prevent cross-site forgery requests
      // and to flag caching bootstrap that Ajax request is being made
      beforeSend: function(xhr) {
        xhr.setRequestHeader("Authcache","1");
      },

      success: function(data, status, xhr) {
        // Fire handlers of individual commands
        $.each(this.commands, function(op) {
          $(this).triggerHandler('result', [data && data[op], status, xhr]);
        });

        $(this).triggerHandler('success', [data, status, xhr]);
      },

      error: function(xhr, status, e) {
        $(this).triggerHandler('error', [status, xhr]);
      },

      complete: function(xhr, status) {
        $(this).triggerHandler('complete', [status, xhr]);
      }
    });

    $(this).triggerHandler('send');
  };

  function AuthcacheAjax() {
    this.requests = {};
  }

  /**
   * Return a pending request associated with the given key. If no request is
   * pending, create a new one.
   */
  AuthcacheAjax.prototype.request = function(key) {
    var req, config, params, me = this;
    key = key || 'default';

    if (!(req = this.requests[key])) {
      params = {};
      config = Drupal.settings.authcacheAjax.requests[key];

      if (config.q) {
        params.q = config.q
      }

      if (key !== 'default') {
        params.g = key;
      }

      params.v = Drupal.settings.authcacheAjax.vers;

      req = new Req(Drupal.settings.basePath + Drupal.settings.pathPrefix, params);

      $(req).bind('send', function() {
        delete me.requests[key];

        // Let debug script know about the new request.
        $(me).triggerHandler('send', req);
      });

      this.requests[key] = req;
    }

    return req;
  };

  /**
   * Schedule one command in the default request.
   */
  AuthcacheAjax.prototype.cmd = function() {
    var cmd = arguments[0];
    var type = Drupal.settings.authcacheAjax.commands[cmd];
    var req = this.request(type);
    return req.command.apply(req, arguments);
  };

  // Create global instance
  $.AuthcacheAjax = new AuthcacheAjax();
}(jQuery));
