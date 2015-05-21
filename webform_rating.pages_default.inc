<?php

/**
* Implementation of hook_default_page_manager_pages().
*/
function webform_rating_default_page_manager_pages() {
  $pages = array();
  $path = drupal_get_path('module', 'webform_rating') . '/pages';
  $files = drupal_system_listing("/\.inc$/", $path, 'name', 0);
  foreach($files as $file) {
    include_once 'pages/' . $file->filename;
    $pages[$page->name] = $page;
   }
  return $pages;
}
