<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-01-05
 * Time: 20:11
 */

/**
 * Implements hook_theme().
 */
function sherlock_theme($existing, $type, $theme, $path) {
  return array(
    'preview_results' => array(
      'variables' => array(
        'output_containers' => NULL,
      ),
      'template' => 'sherlock_preview_results',
      'path' => drupal_get_path('module', 'sherlock') . '/templates',
    ),
  );
}
