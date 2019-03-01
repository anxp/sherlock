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
  $available_hooks = [
    'preview_results' => [
      'variables' => ['output_containers' => NULL,],
      'template' => 'sherlock_preview_results',
      'path' => drupal_get_path('module', 'sherlock') . '/templates',
    ],

    'show_constructed_queries' => [
      'variables' => ['constructed_queries' => NULL,],
      'template' => 'sherlock_show_queries',
      'path' => drupal_get_path('module', 'sherlock') . '/templates',
    ],
  ];

  return $available_hooks;
}
