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
  if (!isset($_POST['market_id'])) {return 'ERROR: Market ID not set.';} //TODO make this return same structure array as normal return.

  //Get marketID to fetch. This ID is passed from frontend by Ajax/POST request. So we check $_POST for it:
  $marketID = $_POST['market_id'];
  unset($_POST['market_id']);

  //Next step - get collection of search URL queries for given $marketID,
  //which are stored in $_SESSION['sherlock_tmp_storage']['constructed_urls_collection'][$marketID]:
  $searchURLs_by_marketID = $_SESSION['sherlock_tmp_storage']['constructed_urls_collection'][$marketID];

  //Construct Object Name:
  $objName = $marketID . '_ItemSniper';

  $snipeRawResults = [];
  for ($i = 0; $i < count($searchURLs_by_marketID); $i++) {
    $sniperObject = new $objName($searchURLs_by_marketID[$i], 5); //Create new object of somemarket_ItemSniper.
    $oneQueryResult = $sniperObject->grabItems();
    unset($sniperObject);
    $snipeRawResults[$searchURLs_by_marketID[$i]] = $oneQueryResult;
  }

  //TODO - Here is perfect place to cache search results ($snipeRawResults)
  //TODO; for given searchURL ($snipeRawResults['key']) in DB.

  $flattened_arr = [];
  foreach ($snipeRawResults as $value) {
    foreach ($value as $end_element) {
      $flattened_arr[] = $end_element;
    }
    unset($end_element);
  }
  unset($value);

  //And finally - in $filteredData we'll have filtered and unique collection of [title, link, price] for given marketID,
  //and this data we'll return to frontend as JSON.
  $filteredData = ArrayFiltration_2D::selectUniqueSubArrays_byField($flattened_arr, 'link');

  //If user specified price range - let's do such filtration!
  $price_from = $_SESSION['sherlock_tmp_storage']['price_from'];
  $price_to = $_SESSION['sherlock_tmp_storage']['price_to'];
  $filteredData = ArrayFiltration_2D::selectSubArrays_byFieldValueRange($filteredData, 'price_value', $price_from, $price_to);

  //At the very last step - cache images (or use already cached if they are exists):
  foreach ($filteredData as &$value) {
    $pic = new Sherlock_FileManager_D7('public://sherlock/img_cache');
    $picExtUrl = $value['thumbnail'];
    $locUrl = $pic->loadRemoteFile($picExtUrl)->saveFileManaged_D7()->getLocalFileUrl_D7();
    $value['thumbnail'] = $locUrl ? $locUrl : '';
    unset($pic);
  }
  unset($value);

  return $filteredData;
}
