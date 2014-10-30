//
// Authcache debug functionality
//
(function ($) {
  // Private variables
  var ajaxCount = 0;
  var timeStart = new Date().getTime();
  var cacheRenderTime = null;
  var info = {
    'Cache Status': 'MISS',
  };

  //
  // Private helper functions
  //

  /**
   * Inject authcache debug widget into the page
   */
  function widget(settings) {
    var alertColor = 'orange';

    $("body").prepend("<div id='authcachedbg' style='max-width: 80em;'><div id='authcache_status_indicator'></div><b><a href='#' id='authcachehide'>Authcache Debug</a></b><div id='authcachedebug' style='display:none;'><div id='authcachedebuginfo'></div></div></div>");
    $("#authcachehide").click(function() {
      $("#authcachedebug").toggle();
      return false;
    });

    // If cache_render cookie is set, we did hit the page cache.
    if ($.cookie("cache_render") && $.cookie("cache_render") != "get") {
      cacheRenderTime = $.cookie("cache_render");
      info['Cache Status'] = 'HIT';

      if (isNaN(parseFloat(cacheRenderTime)) && isFinite(cacheRenderTime)) {
        cacheRenderTime = null;
      }
      else if (cacheRenderTime < 30) {
        alertColor = 'green';
      }
      else if (cacheRenderTime < 100) {
        alertColor = 'orange';
      }
      else if (cacheRenderTime > 100) {
        alertColor = 'red';
      }
    }
    else if (settings.nocacheReason) {
      // Reason for page-exclusion is passed by settings.
      info['Cache Status'] = 'Page EXCLUDED';
      info['Reason'] = settings.nocacheReason;
      alertColor = 'red';
    }
    else if ($.cookie('nocache_reason')) {
      // Reason for cache-cancellation is passed to us using a cookie.
      info['Cache Status'] = 'Caching CANCELLED';
      info['Reason'] = $.cookie('nocache_reason').replace(/\+/g, " ")
      alertColor = 'red';
    }
    else {
      // Test whether page was rendered using boost
      var lastcomment = jQuery("*").contents().filter(function() { return this.nodeType == 8; }).last();
      if (lastcomment.length && lastcomment[0].textContent.match(/Page cached by Boost/)) {
        info['Cache Status'] = 'HIT';
        alertColor = 'green';
      }
    }

    // Add some more settings and status information
    if (settings.key !== undefined) {
      info["Authcache Key"] = settings.key;
    }
    if (settings.props !== undefined) {
      info["Key Properties"] = settings.props;
    }
    if (settings.cacheTime) {
      info["Page Age"] = Math.round(timeStart / 1000 - settings.cacheTime) + " seconds";
    }
    if (cacheRenderTime !== null) {
      info["Cache Render Time"] = cacheRenderTime + " ms";
    }

    $("#authcache_status_indicator").css({"background": alertColor});

    updateInfoFieldset();

    debugTimer();
  }

  /**
   * Install callbacks for authcache_ajax-requests.
   */
  function onRequest(evt, req) {
    $(req).bind('success', function(evt, data, status, xhr) {
      var ajaxLink = 'Request:';
      var legend = "Authcache.ajaxRequest #" + (++ajaxCount);
      var cachectl = xhr.getResponseHeader('Cache-Control');
      var matches;

      // Parse: Cache-Control: max-age=000, must-revalidate
      if (cachectl) {
        matches = cachectl.match(/max-age=(\d+)/);
        if (matches) {
          legend += " (Cached for " + matches.pop() + " seconds)";
        }
      }

      $("#authcachedebug").append(
        "<fieldset><legend><b>" + legend + "</b></legend>" + debugFieldset(ajaxLink, req.params) +
        debugFieldset("Response:", data) +
        "</fieldset>");

        debugTimer();
    });

    $(req).bind('error', function(evt, status, xhr) {
      var text = xhr.responseText;
      $("#authcachedebug").append(debugFieldset("Ajax Response Error ("+status+")", {"ERROR":text.replace(/\n/g,"") }));
    });
  }

  /**
   * Update the info fieldset.
   */
  function updateInfoFieldset() {
    var legend = "Status and Settings" + (($.cookie('drupal_user')) ? " (logged in: "+$.cookie('drupal_user')+')' : '');
    $("#authcachedebuginfo").first().html(debugFieldset(legend, info));
  }

//  /**
//   * Disable caching by setting cookie
//   */
//  Authcachedbg.debugDisable = function() {
//    if (confirm("Are you sure? (You can renable caching by closing and reopening your browser.)")) {
//      $.cookie('nocache', 1);
//      location.reload(true);
//      //setTimeout("location.reload(true)", 1000);
//    }
//    return false;
//  };
//
//  /**
//   * Disable caching by setting cookie
//   */
//  Authcachedbg.debugEnable = function() {
//    $.cookie('nocache', null, {path:'/'});
//    location.reload(true);
//    //setTimeout("location.reload(true)", 1000);
//    return false;
//  };

  /**
   * Display total JavaScript execution time for this file (including Ajax)
   */
  function debugTimer() {
    timeMs = new Date().getTime() - timeStart;
    $("#authcachedebug").append("HTML/JavaScript time: "+timeMs+" ms <hr size=1>");
  }

  /**
   * Helper function (renders HTML fieldset)
   */
  function debugFieldset(title, jsonData) {
    var fieldset = '<div style="clear:both;"></div><fieldset style="float:left;min-width:240px;"><legend>'+title+'</legend>';
    $.each(jsonData, function(key, value) {
      fieldset += "<b>"+key+"</b>: "+JSON.stringify(value).replace(/</g, '&lt;') +'<br>';
    });
    fieldset += '</fieldset><div style="clear:both;">';
    return fieldset;
  }

  /**
   * Attach widget when DOM is ready.
   */
  $(function() {
    if ($.isEmptyObject(Drupal.settings.authcacheDebug)) {
      return;
    }

    widget(Drupal.settings.authcacheDebug);

    if ($.AuthcacheAjax) {
      $($.AuthcacheAjax).bind('send', onRequest);

      // Retrieve additional debug info using an ajax-call
      $.AuthcacheAjax.cmd('debuginfo', Drupal.settings.authcacheDebug.cid, function(evt, response) {
        if ($.isEmptyObject(response)) {
          return;
        }

        info["Page Render Time"] = response.pageRender + " ms";
        if (cacheRenderTime !== null) {
          info["Speedup"] = Math.round((response.pageRender - cacheRenderTime) / cacheRenderTime * 100).toString().replace(/(\d+)(\d{3})/, '$1' + ',' + '$2') + "% increase";
        }
        delete response.pageRender;

        info = $.extend(info, response);

        updateInfoFieldset();
      });
    }
  });

  $(window).load(function() {
    // Reset debug cookies only after all subrequests (images, JS, CSS) are completed.
    $.authcache_cookie("cache_render", "get");
    $.authcache_cookie("nocache_reason", null);
  });
}(jQuery));

/**
 * JSON to String
 * http://www.JSON.org/js.html
 */
if(!this.JSON){JSON={};}
(function(){function f(n){return n<10?'0'+n:n;}
if(typeof Date.prototype.toJSON!=='function'){Date.prototype.toJSON=function(key){return this.getUTCFullYear()+'-'+
f(this.getUTCMonth()+1)+'-'+
f(this.getUTCDate())+'T'+
f(this.getUTCHours())+':'+
f(this.getUTCMinutes())+':'+
f(this.getUTCSeconds())+'Z';};String.prototype.toJSON=Number.prototype.toJSON=Boolean.prototype.toJSON=function(key){return this.valueOf();};}
var cx=/[\u0000\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,escapable=/[\\\"\x00-\x1f\x7f-\x9f\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,gap,indent,meta={'\b':'\\b','\t':'\\t','\n':'\\n','\f':'\\f','\r':'\\r','"':'\\"','\\':'\\\\'},rep;function quote(string){escapable.lastIndex=0;return escapable.test(string)?'"'+string.replace(escapable,function(a){var c=meta[a];return typeof c==='string'?c:'\\u'+('0000'+a.charCodeAt(0).toString(16)).slice(-4);})+'"':'"'+string+'"';}
function str(key,holder){var i,k,v,length,mind=gap,partial,value=holder[key];if(value&&typeof value==='object'&&typeof value.toJSON==='function'){value=value.toJSON(key);}
if(typeof rep==='function'){value=rep.call(holder,key,value);}
switch(typeof value){case'string':return quote(value);case'number':return isFinite(value)?String(value):'null';case'boolean':case'null':return String(value);case'object':if(!value){return'null';}
gap+=indent;partial=[];if(Object.prototype.toString.apply(value)==='[object Array]'){length=value.length;for(i=0;i<length;i+=1){partial[i]=str(i,value)||'null';}
v=partial.length===0?'[]':gap?'[\n'+gap+
partial.join(',\n'+gap)+'\n'+
mind+']':'['+partial.join(',')+']';gap=mind;return v;}
if(rep&&typeof rep==='object'){length=rep.length;for(i=0;i<length;i+=1){k=rep[i];if(typeof k==='string'){v=str(k,value);if(v){partial.push(quote(k)+(gap?': ':':')+v);}}}}else{for(k in value){if(Object.hasOwnProperty.call(value,k)){v=str(k,value);if(v){partial.push(quote(k)+(gap?': ':':')+v);}}}}
v=partial.length===0?'{}':gap?'{\n'+gap+partial.join(',\n'+gap)+'\n'+
mind+'}':'<br>{'+partial.join(',<br>')+'}';gap=mind;return v;}}
if(typeof JSON.stringify!=='function'){JSON.stringify=function(value,replacer,space){var i;gap='';indent='';if(typeof space==='number'){for(i=0;i<space;i+=1){indent+=' ';}}else if(typeof space==='string'){indent=space;}
rep=replacer;if(replacer&&typeof replacer!=='function'&&(typeof replacer!=='object'||typeof replacer.length!=='number')){throw new Error('JSON.stringify');}
return str('',{'':value});};}
if(typeof JSON.parse!=='function'){JSON.parse=function(text,reviver){var j;function walk(holder,key){var k,v,value=holder[key];if(value&&typeof value==='object'){for(k in value){if(Object.hasOwnProperty.call(value,k)){v=walk(value,k);if(v!==undefined){value[k]=v;}else{delete value[k];}}}}
return reviver.call(holder,key,value);}
cx.lastIndex=0;if(cx.test(text)){text=text.replace(cx,function(a){return'\\u'+
('0000'+a.charCodeAt(0).toString(16)).slice(-4);});}
if(/^[\],:{}\s]*jQuery/.test(text.replace(/\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g,'@').replace(/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g,']').replace(/(?:^|:|,)(?:\s*\[)+/g,''))){j=eval('('+text+')');return typeof reviver==='function'?walk({'':j},''):j;}
throw new SyntaxError('JSON.parse');};}})();
