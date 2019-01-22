<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-01-05
 * Time: 15:31
 */

//========================================= HANDLERS FOR ALL FORM BUTTONS ==============================================

function btn_addterm_handler($form, &$form_state) {
  $pressed_btn_name = $form_state['triggering_element']['#name'];
  if ($pressed_btn_name != 'btn_addterm') {return;}

  new_block($form_state);

  $form_state['rebuild'] = TRUE;
}

function btn_addvariation_handler($form, &$form_state) {
  $pressed_btn_name = $form_state['triggering_element']['#name'];
  if (!((bool) strstr($pressed_btn_name, 'btn_addvariation'))) {return;}

  $btn_id = (int) explode('-', $pressed_btn_name)[1];
  $variationNo = isset($form_state['values']['query_constructor_block']['KEYWORD-'.$btn_id]['VALUES']) ? count($form_state['values']['query_constructor_block']['KEYWORD-'.$btn_id]['VALUES']) : 0;
  $newfield = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => 'Variation '.$btn_id.'/'.$variationNo,
  );

  array_push($form_state['user_added']['KEYWORD-'.$btn_id]['VALUES'], $newfield);

  $form_state['rebuild'] = TRUE;
}

function btn_reset_handler($form, &$form_state) {
  $pressed_btn_name = $form_state['triggering_element']['#name'];
  if ($pressed_btn_name != 'btn_reset') {return;}

  unset ($form_state['user_added']);
  $form_state['rebuild'] = FALSE;
}

function btn_preview_validate_handler($form, &$form_state) {
  $resources = $form_state['values']['resources_chooser'];
  $some_resources_selected = FALSE;
  foreach ($resources as $value) {
    if ($value !== 0) {
      $some_resources_selected = TRUE;
      break;
    }
  }
  if (!$some_resources_selected) {
    form_set_error('resources_chooser', 'Please, select at least one resource where to search.');
  }
}

function btn_preview_handler($form, &$form_state) {
  $pressed_btn_name = $form_state['triggering_element']['#name'];
  if ($pressed_btn_name != 'btn_preview') {return;}

  //Get flag which indicates - search title only or body too:
  $check_description_too = ((int) $form_state['values']['dscr_chk']) === 0 ? FALSE : TRUE;

  //Save all block values\variations to separate array. Strictly speaking, we don't need to check if key is starts with 'KEYWORD-', because all 1st level keys are 'KEYWORD-'.
  //But left this check just for reliability.
  $block_values = array();
  foreach ($form_state['values']['query_constructor_block'] as $key => $value) {
    if (startsWith($key, 'KEYWORD-')) {
      $block_values[] = $value['VALUES']; //Each value of $block_values is ARRAY with combinations of one given keyword (like sony, soni).
    }
  }

  //Now let's request list of all resources and their settings.
  $resources_list = [];
  $resources_list = get_available_fleamarkets(TRUE); //See sherlock.module for this function declaration.

  //Array with all search URLs for all user-specified resources. This collection of URL we will use to cURL each of them at the next step.
  //All values are splitted by nested arrays. Keys to nested arrays are names of the resources.
  $constructed_urls_collection = array();
  foreach ($form_state['values']['resources_chooser'] as $key => $value) { //$key here is flea market ID, like olx, bsp, skl
    if ($value !== 0) { //we take into consideration only checked resources, if checkbox unchecked its value == 0
      $prefix = $resources_list[$key]::getSearchURL();
      $glue = $resources_list[$key]::getWordsGlue();
      $suffix = $check_description_too ? $resources_list[$key]::getSuffixDescrChk() : '';
      $constructed_urls_collection[$key] = generate_all_possible_strings($block_values, $prefix, $glue, $suffix);
    }
  }
  unset($key, $value);

  $_SESSION['sherlock_tmp_storage'] = [];
  $_SESSION['sherlock_tmp_storage']['constructed_urls_collection'] = $constructed_urls_collection;

  //Save selected (ONLY SELECTED!) fleamarkets to $_SESSION,
  //(we'll take them by ajax from there in a moment from frontend, see $items['sherlock/selected-markets'] endpoint in sherlock.module):
  foreach ($form_state['values']['resources_chooser'] as $key => $value) {
    if ($value !== 0) {
      $_SESSION['sherlock_tmp_storage']['selected_markets'][] = $key;
    }
  }
  unset($key, $value);

  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- LET'S THE PARTY BEGIN! *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
  //At this point, we have enough info at $_SESSION['sherlock_tmp_storage'] to start fetch and parsing process:
  //$_SESSION['sherlock_tmp_storage']['selected_markets'] contains just user-selected markets to fetch,
  //$_SESSION['sherlock_tmp_storage']['constructed_urls_collection'] contains sub-arrays with collection of search queries for each market.
  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

  //$form_state['redirect'] = 'sherlock/preview-results';
  $form_state['storage']['step'] = 2;
  $form_state['rebuild'] = TRUE;
}
