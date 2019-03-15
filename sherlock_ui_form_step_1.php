<?php
/**
 * This file contains form for first step - where user construct his own search query.
 */

$fleamarket_objects = get_available_fleamarkets(TRUE);

//Print out supported flea-markets. TODO: maybe this output better to do with theme function and template file?
$formatted_list = [];

foreach ($fleamarket_objects as $object) {
  $current_market_id = $object::getMarketId();
  $current_market_name = $object::getMarketName();
  $current_market_url = $object::getBaseURL();
  $formatted_list[$current_market_id] = $current_market_name . ' [<a target="_blank" href="' . $current_market_url . '">' . t('Open this market\' website in new tab') . '</a>]';
}
unset($object);

$form['resources_chooser'] = [
  '#type' => 'checkboxes',
  '#options' => $formatted_list,
  '#title' => 'Choose flea-markets websites to search on',
];

$form['query_constructor_block'] = [
  '#type' => 'container',
  '#attributes' => [
    'class' => ['query_constructor_block'],
  ],
];

//Don't try to combine next two IFs to one block, they need to be as they written!:
//When user request form first time -> we show him (new) empty block with one field for text string:
if(!isset($form_state['user_added'])) {
  sherlock_new_block($form_state);
}

//If user already begun to build the query (and so, $form_state['user_added'] exists) -> we show him already filled blocks and fields:
if(isset($form_state['user_added'])) {
  foreach ($form_state['user_added'] as $key => $value) {
    $form['query_constructor_block'][$key] = $value;
  }
}

//---------------- Additional search parameters block: -----------------------------------------------------------------
$form['additional_params'] = [
  '#type' => 'fieldset',
  '#title' => 'Additional search parameters',
  '#collapsible' => TRUE,
  '#collapsed' => TRUE,
];

//By default, search performs only in headers, but some resources (OLX and Skylots, but not Besplatka) also supports
//search in body. Technically, it adds specific suffix to URL.
$form['additional_params']['dscr_chk'] = [
  '#type' => 'checkbox',
  '#title' => 'Search in descriptions too (if resource supports).',
];

$form['additional_params']['filter_by_price'] = [
  '#type' => 'checkbox',
  '#title' => 'Filter items by price:',
];

$form['additional_params']['price_from'] = [
  '#type' => 'textfield',
  '#title' => 'Price from:',
  '#default_value' => '',
  '#size' => 10,
  '#maxlength' => 20,
  '#description' => 'UAH', //'Enter the minimal price, at which (or higher) item will be selected.',
  '#prefix' => '<div class="container-inline">',
  '#suffix' => '</div>',
];

$form['additional_params']['price_to'] = [
  '#type' => 'textfield',
  '#title' => 'Price to:',
  '#default_value' => '',
  '#size' => 10,
  '#maxlength' => 20,
  '#description' => 'UAH', //'Enter the maximum acceptable price for items.',
  '#prefix' => '<div class="container-inline">',
  '#suffix' => '</div>',
];

//----------------------------------------------------------------------------------------------------------------------

//Create a DIV wrapper for button(s) - according to Drupal 7 best practices recommendations:
$form['first_step_buttons_wrapper'] = ['#type' => 'actions'];

//...and three main controls buttons () in this wrapper:
$form['first_step_buttons_wrapper']['btn_addterm'] = [
  '#type' => 'submit',
  '#value' => 'Add Term',
  '#name' => 'btn_addterm',
  '#submit' => [
    'btn_addterm_handler',
  ],
  '#ajax' => [
    'callback' => 'sherlock_updatedform_return',
  ],
];

$form['first_step_buttons_wrapper']['btn_preview'] = [
  '#type' => 'submit',
  '#value' => 'Preview Results',
  '#name' => 'btn_preview',
  '#validate' => [
    'btn_preview_validate_handler'
  ],
  '#submit' => [
    'btn_preview_handler',
  ],
];

$form['first_step_buttons_wrapper']['btn_reset'] = [
  '#type' => 'submit',
  '#value' => 'Reset All',
  '#name' => 'btn_reset',
  '#limit_validation_errors' => [], //We don't want validate any errors for this submit button, because this is RESET button!
  '#submit' => [
    'btn_reset_handler',
  ],
];
