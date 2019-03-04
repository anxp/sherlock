<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-01-05
 * Time: 15:28
 */

function sherlock_searchform($form, &$form_state) {
  $fleamarket_objects = get_available_fleamarkets(TRUE);

  //If form is just created -> we will be on the first step. If user already explored the form -> get current step.
  $step = (isset($form_state['storage']['step'])) ? intval($form_state['storage']['step']) : 1;

  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="sherlock-search-form">';
  $form['#suffix'] = '</div>';

  //Depending on which step we are on, show corresponding part of the form:
  switch ($step) {
    //Show part of the form for step 1:
    case 1:
      drupal_set_title('What are you looking for? Create your perfect search query!');
      //Print out supported flea-markets. TODO: maybe this output better to do with theme function and template file?
      $formatted_list = [];

      foreach ($fleamarket_objects as $object) {
        $current_market_id = $object::getMarketId();
        $current_market_name = $object::getMarketName();
        $current_market_url = $object::getBaseURL();
        $formatted_list[$current_market_id] = $current_market_name . ' [<a target="_blank" href="' . $current_market_url . '">' . t('Open this market\' website in new tab') . '</a>]';
      }
      unset($object);

      $form['resources_chooser'] = array(
        '#type' => 'checkboxes',
        '#options' => $formatted_list,
        '#title' => 'Choose flea-markets websites to search on',
      );

      $form['query_constructor_block'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('query_constructor_block'),
        ),
      );

      //Don't try to combine next two IFs to one block, they need to be as they written!:
      //The first:
      if(!isset($form_state['user_added'])) {
        new_block($form_state);
      }

      //The second:
      if(isset($form_state['user_added'])) {
        foreach ($form_state['user_added'] as $key => $value) {
          $form['query_constructor_block'][$key] = $value;
        }
      }

      //By default, search performs only in headers, but some resources (OLX and Skylots, but not Besplatka) also supports
      //search in body. Technically, it adds specific suffix to URL.
      $form['dscr_chk'] = array(
        '#type' => 'checkbox',
        '#title' => 'Search not only in titles, but in descriptions too (if resource supports).',
      );

      //Create a DIV wrapper for button(s) - according to Drupal 7 best practices recommendations:
      $form['first_step_buttons_wrapper'] = array('#type' => 'actions');

      //...and three main controls buttons () in this wrapper:
      $form['first_step_buttons_wrapper']['btn_addterm'] = array(
        '#type' => 'submit',
        '#value' => 'Add Term',
        '#name' => 'btn_addterm',
        '#submit' => array(
          'btn_addterm_handler',
        ),
        '#ajax' => array(
          'callback' => 'updatedform_return',
        ),
      );

      $form['first_step_buttons_wrapper']['btn_preview'] = array(
        '#type' => 'submit',
        '#value' => 'Preview Results',
        '#name' => 'btn_preview',
        '#validate' => array(
          'btn_preview_validate_handler'
        ),
        '#submit' => array(
          'btn_preview_handler',
        ),
      );

      $form['first_step_buttons_wrapper']['btn_reset'] = array(
        '#type' => 'submit',
        '#value' => 'Reset All',
        '#name' => 'btn_reset',
        '#limit_validation_errors' => array(), //We don't want validate any errors for this submit button, because this is RESET button!
        '#submit' => array(
          'btn_reset_handler',
        ),
      );

      break;

    // ----- STEP 2. Here we show results and give user ability to save his search for future use. ---------------------
    case 2:
      drupal_set_title('Preview and save results.');

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

      break;
  }

  return $form;
}

function new_block(&$form_state) {
  $num_elements = isset($form_state['user_added']) ? count($form_state['user_added']) : 0;

  $form_state['user_added']['KEYWORD-'.$num_elements] = array(
    '#type' => 'fieldset',
    '#title' => 'KEYWORD-'.$num_elements,
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  //This is nested block\container for variation values only (with a view to separate values from button)
  $form_state['user_added']['KEYWORD-'.$num_elements]['VALUES'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form_state['user_added']['KEYWORD-'.$num_elements]['VALUES'][0] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => 'Variation '.$num_elements.'/'.'0',
  );

  //Create new DIV wrapper for Add Variation button (according to Drupal 7 best practices recommendations):
  $form_state['user_added']['KEYWORD-'.$num_elements]['add_variation_button_wrapper'] = array('#type' => 'actions');

  //...and new button inside this wrapper:
  $form_state['user_added']['KEYWORD-'.$num_elements]['add_variation_button_wrapper']['btn_addvariation-'.$num_elements] = array(
    '#type' => 'submit',
    '#value' => 'Add Variation',
    '#name' => 'btn_addvariation-'.$num_elements,
    '#submit' => array(
      'btn_addvariation_handler',
    ),
    '#ajax' => array(
      'callback' => 'updatedform_return',
    ),
  );
}

function updatedform_return($form, $form_state) {
  $commands = array();
  $commands[] = ajax_command_replace('#sherlock-search-form', drupal_render($form));
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
