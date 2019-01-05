<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2018-12-23
 * Time: 22:43
 */
function generate_all_possible_strings($block_values, $prefix, $glue, $suffix='') {
  $block_values_local = $block_values;

  //Trim values of input array for the case if user left space at left or at right:
  foreach ($block_values_local as &$value) {
    foreach ($value as &$subvalue) {
      $subvalue = trim($subvalue);
    }
  }
  unset ($value, $subvalue);

  $variations_array = array();
  $variations_array = array_shift($block_values_local);
  while (!empty($block_values_local)) {
    $array_A = $variations_array;
    $variations_array = array();
    $array_B = array_shift($block_values_local);

    foreach ($array_A as $value_A) {
      foreach ($array_B as $value_B) {
        $variations_array[] = $value_A.$glue.$value_B;
      }
    }
  }

  //If spaces are present -> replace them with 'glue' symbol
  foreach ($variations_array as &$value) {
    $value = preg_replace('/\s+/', $glue, $value);
  }
  unset($value);

  //Make shure that all strings in array are unique:
  $variations_array = array_unique($variations_array);

  //Reset indexing to 0, 1, 2, 3, 4 ... , because if array_unique() deleted some values, we will have missing indexes:
  $variations_array = array_values($variations_array);

  //Add prefix (usually address of the site and search invokation part like https://www.olx.ua/list/q-) and suffix (if available for this website)
  foreach ($variations_array as &$value) {
    $value = $prefix.$value.$suffix;
  }
  unset($value);

  return $variations_array;
}

function sherlock_debug_msg($x) {
  drupal_set_message('<pre>'.print_r($x, true).'</pre>');
}

//Some routine functions copypasted from stackoverflow :)
function startsWith($haystack, $needle) {
  $length = strlen($needle);
  return (substr($haystack, 0, $length) === $needle);
}

function endsWith($haystack, $needle) {
  $length = strlen($needle);
  if ($length == 0) {
    return true;
  }

  return (substr($haystack, -$length) === $needle);
}
