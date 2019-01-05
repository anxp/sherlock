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
