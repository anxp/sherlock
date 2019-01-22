<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-01-05
 * Time: 19:57
 */

function get_demo_content_ajax() {
  $commands[] = ajax_command_append('#olx-output-block', '<p>Hello there! This is ajax response!</p>');
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

function get_selected_markets_ajax() {
  return ($_SESSION['sherlock_tmp_storage']['selected_markets']);
}

function fetch_market_ajax() {
  //Get marketID to fetch. This ID is passed from frontend by Ajax/POST request. So we check $_POST for it:
  $marketID = $_POST['market_id'];

  //Next step - get collection of search URL queries for given $marketID,
  //which are stored in $_SESSION['sherlock_tmp_storage']['constructed_urls_collection'][$marketID]:
  $marketID_searchURL_collection = $_SESSION['sherlock_tmp_storage']['constructed_urls_collection'][$marketID];

  //Construct Object Name:
  $objName = $marketID . '_ItemSniper';

  $snipeResults = [];
  for ($i = 0; $i < count($marketID_searchURL_collection); $i++) {
    $sniperObject = new $objName($marketID_searchURL_collection[$i], 5); //Create new object of somemarket_ItemSniper.
    $tmp = $sniperObject->grabItems();
    unset($sniperObject);
    foreach ($tmp as $value) {
      $snipeResults[] = $value;
    }
    unset($value);
  }

  //TODO - filter duplicates by 'link' key.
  
  sherlock_debug_msg($snipeResults);


  $resp_str = 'This is response from ' . $marketID;
  return $resp_str;
}
