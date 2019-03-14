<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-01-05
 * Time: 15:28
 */

function sherlock_searchform($form, &$form_state) {
  //If form is just created -> we will be on the first step. If user already explored the form -> get current step.
  $step = (isset($form_state['storage']['step'])) ? intval($form_state['storage']['step']) : 1;

  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="sherlock-search-form">';
  $form['#suffix'] = '</div>';

  //Depending on which step we are on, show corresponding part of the form:
  switch ($step) {
    // ----- STEP 1. Here we show constructor and give user ability to make his own search queries. --------------------
    case 1:
      drupal_set_title('What are you looking for? Create your perfect search query!');

      include 'sherlock_ui_form_step_1.php';

      break;

    // ----- STEP 2. Here we show results and give user ability to save his search for future use. ---------------------
    case 2:
      drupal_set_title('Preview and save results.');

      include 'sherlock_ui_form_step_2.php';

      break;
  }

  return $form;
}

function sherlock_new_block(&$form_state) {
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
      'callback' => 'sherlock_updatedform_return',
    ),
  );
}

function sherlock_updatedform_return($form, $form_state) {
  $commands = [];
  $commands[] = ajax_command_replace('#sherlock-search-form', drupal_render($form));
  return [
    '#type' => 'ajax',
    '#commands' => $commands,
  ];
}
