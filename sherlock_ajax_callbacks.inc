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
  return $_SESSION['sherlock_tmp_storage']['selected_markets'];
}
