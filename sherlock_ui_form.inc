<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-01-05
 * Time: 15:28
 */

function sherlock_searchform($form, &$form_state) {
  $fleamarket_objects = get_available_fleamarkets();

  //If form is just created -> we will be on the first step. If user already explored the form -> get current step.
  $step = (isset($form_state['storage']['step']) && !empty($form_state['storage']['step'])) ? intval($form_state['storage']['step']) : 1;

  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="sherlock-search-form">';
  $form['#suffix'] = '</div>';

  $form['current-step-title-wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('current-step-title-wrapper'),
    ),
  );

  $form['current-step-title-wrapper']['title'] = array(
    '#markup' => '<h3>'.get_form_title($step).'</h3>',
  );

  //Depending on which step we are on, show corresponding part of the form:
  switch ($step) {
    //Show part of the form for step 1:
    case 1:
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

    //Show part of the form for step 2:
    case 2:
      sherlock_debug_msg($form_state['values']['resources_chooser']);
      //At first, plug in jQuery AJAX additions for this part of the form:
      $form['#attached']['js'] = array(
        drupal_get_path('module', 'sherlock') . '/ajax_content_request.js',
      );

      //Second, attach drupal.ajax library, this is the same as do it by function: drupal_add_library('system', 'drupal.ajax');
      $form['#attached']['library'] = array(
        array('system', 'drupal.ajax')
      );

      $form['#attached']['css'] = [
        drupal_get_path('module', 'sherlock') . '/templates/content_output_tabs.css',
      ];

      $output_containers = [];
      foreach ($form_state['values']['resources_chooser'] as $market_id) { //'olx', 'bsp', 'skl', or 0 (zero).
        //Let's create div-container for every checked resource, because we need place where to output parse result
        if ($market_id !== 0) {
          foreach ($fleamarket_objects as $object) {
            if ($object::getMarketId() === $market_id) {
              $output_containers[$market_id]['market_id'] = $market_id;
              $output_containers[$market_id]['container_title'] = $object::getMarketName();
              $output_containers[$market_id]['container_id'] = $market_id.'-output-block';
            }
          }
          unset($object);
        }
      }

      $form['preview_results_area'] = array(
        '#type' => 'markup',
        '#markup' => theme('preview_results', array('output_containers' => $output_containers)),
        '#prefix' => '<div id="preview-results-parent-block">',
        '#suffix' => '</div>',
      );

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

//We have a multistep form, so we need different titles for every step. This function returns title depends on current step:
function get_form_title(int $step) {
  $title = '';
  switch ($step) {
    case 1:
      $title = 'Step 1: Make a search query.';
      break;

    case 2:
      $title = 'Step 2: Preview results.';
      break;

    default:
      $title = 'No title for this step.';
  }

  return $title;
}
