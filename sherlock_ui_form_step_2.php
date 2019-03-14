<?php
/**
 * This file contains form for second step - where user can see results and save or update his queries to database.
 */

$fleamarket_objects = get_available_fleamarkets(TRUE);

//Attach JS and CSS for first block - 'List of constructed search queries':
$form['#attached']['js'][] = drupal_get_path('module', 'sherlock') . '/templates/sherlock_show_queries.js';
$form['#attached']['css'][] = drupal_get_path('module', 'sherlock') . '/templates/sherlock_show_queries.css';
//----------------------------------------------------------------------------------------------------------------

//Attach JS and CSS for second block - with tabs and tables for output gathered information:
$form['#attached']['js'][] = drupal_get_path('module', 'sherlock') . '/ajax_content_request.js'; //Plug in jQuery AJAX additions for this part of the form.
$form['#attached']['library'][] = ['system', 'drupal.ajax',]; //Attach drupal.ajax library, this is the same as do it by function: drupal_add_library('system', 'drupal.ajax');
$form['#attached']['css'][] = drupal_get_path('module', 'sherlock') . '/templates/sherlock_content_output_tabs.css';
$form['#attached']['css'][] = drupal_get_path('module', 'sherlock') . '/templates/sherlock_content_output_table.css';
//----------------------------------------------------------------------------------------------------------------

$output_containers = [];
foreach ($form_state['values']['resources_chooser'] as $market_id) { //'olx', 'bsp', 'skl', or 0 (zero).
  //Let's create div-container for every checked resource, because we need place where to output parse result
  if ($market_id === 0) {continue;}
  $output_containers[$market_id]['market_id'] = $market_id;
  $output_containers[$market_id]['container_title'] = $fleamarket_objects[$market_id]::getMarketName();
  $output_containers[$market_id]['container_id'] = $market_id.'-output-block';
}

//Prepare associative array with constructed search queries to show to user. Keys of array are normal (not short!) flea-market names.
$constructed_urls_collection = [];
foreach ($_SESSION['sherlock_tmp_storage']['constructed_urls_collection'] as $key => $value) {
  $user_friendly_key = $fleamarket_objects[$key]::getMarketName();
  $constructed_urls_collection[$user_friendly_key] = $value;
}
unset($key, $value);

$form['constructed_search_queries'] = [
  '#type' => 'markup',
  '#markup' => theme('show_constructed_queries', ['constructed_queries' => $constructed_urls_collection,]),
  '#prefix' => '<div id="constructed-queries-block">',
  '#suffix' => '</div>',
];

$form['preview_results_area'] = [
  '#type' => 'markup',
  '#markup' => theme('preview_results', ['output_containers' => $output_containers,]),
  '#prefix' => '<div id="preview-results-parent-block">',
  '#suffix' => '</div>',
];