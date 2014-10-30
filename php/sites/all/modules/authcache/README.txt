
===========================================
Authenticated User Page Caching (Authcache)
===========================================

The Authcache module offers page caching for both anonymous users and logged-in
authenticated users. This allows Drupal/PHP to spend only 1-2 milliseconds
serving pages and greatly reduces server resources.

Please visit:

  http://drupal.org/project/authcache

For information, updates, configuration help, and support.

============
Installation
============

1. Enable the authcache module. Also enable all authcache submodules providing
   support for enabled core and contrib modules like "Authcache Poll" if you
   are using the "Poll" module or "Authcache Forum" if the "Forum" module is
   active. Note: Some functionality is only enabled when "Authcache AJAX" is
   also enabled.

2. Setup a Drupal cache handler module (optional, but strongly recommended for
   vastly improved performance)

   Download and enable a cache handler module, such as:

   -- Memcache API @ http://drupal.org/project/memcache
   -- Filecache @ http://drupal.org/project/filecache

3. Open your settings.php file and configure the cache backends. If the AJAX
   module is enabled, you also need to include the cache backend from the
   authcache_ajax module.

   Here are some examples:

   ---------------
   MEMCACHE MODULE
   ---------------
   $conf['memcache_servers']  = array('localhost:11211' => 'default');

   $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
   $conf['cache_backends'][] = 'sites/all/modules/authcache/modules/authcache_ajax/authcache_ajax.inc';
   $conf['cache_backends'][] = 'sites/all/modules/authcache/authcache.inc';
   $conf['cache_class_cache_page'] = 'MemCacheDrupal';

   ----------------
   FILECACHE MODULE
   ----------------

   $conf['cache_backends'][] = 'sites/all/modules/filecache/filecache.inc';
   $conf['cache_backends'][] = 'sites/all/modules/authcache/modules/authcache_ajax/authcache_ajax.inc';
   $conf['cache_backends'][] = 'sites/all/modules/authcache/authcache.inc';
   $conf['cache_class_cache_page'] = 'DrupalFileCache';

   ------------------------------------------------------------------------
   If you are using a cache module other than FileCache / Memcache, or if the
   module is in a different parent directory than Authcache, define the cache
   include path using:
   -------------------------------------------------------------------------

   $conf['cache_backends'][] = './sites/path/to/module/cacheinclude.inc';
   $conf['cache_backends'][] = 'sites/all/modules/authcache/authcache.inc';
   $conf['cache_class_cache_page'] = 'your_cache_hander_name';

   -------------------------------------------------------------------
   If no cache handler is setup or defined, Authcache will fallback to Drupal
   core database cache tables and "Authcache Debug" will say "cache_class:
   DrupalDatabaseCache".

   If you are experimenting with multiple caching systems (db, apc, memcache),
   make sure to clear the cache each time you switch to remove stale data.

4. Goto Configuration > System > Authcache and specify the cacheable user roles.

5. Modify your theme by tweaking user-customized elements (the final HTML
   must be the same for each user role). Use hook_preprocess to replace user
   specific content with <span>-tags which you can address using JavaScript.
   See the numerous examples in the modules and example directories.

=================
CACHE FLUSH NOTES
=================

Page cache is cleared when cron.php is executed. This is normal Drupal core
behavior. Using the Elysia Cron module it is possible to suppress overzealous
cache clearing by running system_cron on a slower pace than other cron jobs.

See:
  -- Elysia Cron @ http://drupal.org/project/elysia_cron

========================
Authcache Example Module
========================

Please review the examples in the modules and example directories for a
demonstration on how to alter the behavior of other modules such that their
output becomes cacheable for logged in users.

======
Author
======

Developed & maintained by Jonah Ellison.

Email: jonah [at] httpremix.com
Drupal: http://drupal.org/user/217669

Initial D7 port by Simon Gardner
Email: slgard@gmail.com
Drupal: http://drupal.org/user/620692

Version 7.x-2.x by Lorenz Schori
Drupal: http://drupal.org/user/63999

================
Credits / Thanks
================

- "Cache Router" module (Steve Rude) for the caching system/API
- "Boost" module (Arto Bendiken) for some minor code & techniques
