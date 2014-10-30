<?php

/**
 * Given a set of elements, return all possible subsets with a size of k.
 *
 * If the input looks like this:
 *   $set = array('a', 'b', 'c', 'd')
 *   $k = 2
 *
 * The result will be:
 *   $result = array(
 *     array('a', 'b'),
 *     array('a', 'c'),
 *     array('a', 'd'),
 *     array('b', 'c'),
 *     array('b', 'd'),
 *     array('c', 'd'),
 *   );
 */
function _authcache_comb_k($set, $k) {
  if ($k >= count($set)) {
    return array($set);
  }
  else if ($k <= 1) {
    return array_chunk($set, 1);
  }

  $result = array();
  while($k <= count($set)) {
    $head = array_shift($set);
    $tails = _authcache_comb_k($set, $k-1);
    foreach ($tails as $tail) {
      array_unshift($tail, $head);
      array_push($result, $tail);
    }
  }

  return $result;
}

/**
 * Given a set of elements, return all possible subsets.
 */
function _authcache_comb($set) {
  $result = array();

  for ($k=1; $k <= count($set); $k++) {
    $result = array_merge($result, _authcache_comb_k($set, $k));
  }

  return $result;
}

/**
 * Generate records of key-value pairs by combining the choices defined by the
 * given template.
 *
 * If the input looks like this:
 *   $template = array(
 *     'x' => array(1, 2, 3),
 *     'y' => array('a', 'b'),
 *   );
 *
 * The result consists of an array with 6 elements:
 *   $result = array(
 *     array('x' => 1, 'y' => 'a'),
 *     array('x' => 2, 'y' => 'a'),
 *     array('x' => 3, 'y' => 'a'),
 *     array('x' => 1, 'y' => 'b'),
 *     array('x' => 2, 'y' => 'b'),
 *     array('x' => 3, 'y' => 'b'),
 *   );
 */
function _authcache_expand($template) {
  $n = 1;
  $div = array();
  $mod = array();

  foreach ($template as $key => $choices) {
    $c = count($choices);
    $div[$key] = $n;
    $mod[$key] = $c;
    $n *= $c;
  }

  $result = array();

  for ($i = 0; $i < $n; $i++) {
    foreach ($template as $key => $choices) {
      $j = ((int)($i / $div[$key])) % $mod[$key];
      $result[$i][$key] = $choices[$j];
    }
  }

  return $result;
}
