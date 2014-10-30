<?php

class AuthcacheStatisticsUpdateCommand extends AuthcacheAjaxCommandHandlerBase {
  /**
   * Node counter and access log statistics
   * @see statistics.module
   */
  function run($op, $vars) {
    include_once './modules/statistics/statistics.module';
    statistics_exit();
  }
}
