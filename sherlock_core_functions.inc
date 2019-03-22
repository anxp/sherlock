<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2018-12-23
 * Time: 22:43
 */

function generate_all_possible_combinations (array $block_values): array {
  //FIRST STEP: If we found string as keyword -> we explode it to array of words, so it will be more easier to implode back.
  //If string contains only one word -> we also do array from it, just array with only one element:
  $block_values_count = count($block_values); //Number of blocks/terms. We don't take into consideration term variations here. Just count number of blocks with term (terms variations) inside.
  $block_values_rebuilt = [];

  for ($i = 0; $i < $block_values_count; $i++) {
    $current_term_count = count($block_values[$i]);

    for ($j = 0; $j < $current_term_count; $j++) {
      $tmp = explode(' ', $block_values[$i][$j]);
      $tmp_count = count($tmp);

      for ($k = 0; $k < $tmp_count; $k++) {
        $tmp[$k] = trim($tmp[$k], ' .,?!');
      }

      $block_values_rebuilt[$i][$j] = $tmp;
    }
  }
  //--------------------------------------------------------------------------------------------------------------------

  //SECOND STEP: Compose all possible combinations from keywords:
  $combinations_array = [];
  $combinations_array = array_shift($block_values_rebuilt);

  while (!empty($block_values_rebuilt)) {
    $array_A = $combinations_array;
    $combinations_array = [];
    $array_B = array_shift($block_values_rebuilt);

    foreach ($array_A as $value_A) {
      foreach ($array_B as $value_B) {
        $combinations_array[] = array_merge($value_A, $value_B);
      }
      unset($value_B);
    }
    unset($value_A);
  }
  //--------------------------------------------------------------------------------------------------------------------

  //THIRD STEP: Remove duplicates. a) remove duplicates inside each set; b) remove duplicated sets (if any):
  //Every element in $combinations_array is an array of keywords which will form a one string (one search query).
  //Let's check every set of keywords (every element of $combinations_array) for duplicates:
  $combinations_array_count = count($combinations_array);
  for ($i = 0; $i < $combinations_array_count; $i++) {
    $combinations_array[$i] = array_unique($combinations_array[$i]);
  }

  //And finally, let's check that all elements of $combinations_array are unique comparing with each other. Yes, we need to compare arrays!
  for ($i = 0; $i < $combinations_array_count; $i++) {
    for ($j = 0; $j < $combinations_array_count; $j++) {
      if ($i === $j) {continue;} //Obviously, we don't want compare element with itself :)
      if (array_diff($combinations_array[$i], $combinations_array[$j]) === [] && count($combinations_array[$i]) === count($combinations_array[$j])) {
        $combinations_array[$j] = []; //If duplicate found -> replace it with empty array.
      }
    }
  }
  //Select only NOT EMPTY elements:
  $filtered_array = array_filter($combinations_array, function ($value) {return empty($value) ? FALSE : TRUE;});

  return $filtered_array;
}
/*
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
*/
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

function sherlock_array_unique_by_field($input_array, $field_to_check) {
  if (!is_array($input_array)) {return [];}

  $tmp_arr = [];
  foreach ($input_array as $key => $value) {
    $tmp_arr[$key] = $value[$field_to_check];
  }
  unset($key, $value);

  $tmp_arr = array_unique($tmp_arr);

  $filtered_arr = [];
  foreach ($tmp_arr as $key => $value) {
    $filtered_arr[] = $input_array[$key];
  }
  unset($key, $value);

  return $filtered_arr;
}

function sherlock_get_random_string($length) {
  $characters = 'abcdefghijklmnopqrstuvwxyzABCDERFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*';
  $randomString = '';
  for ($i = 0; $i < $length; $i++) {
    $randomString .= $characters[mt_rand(0, strlen($characters)-1)];
  }
  return $randomString;
}