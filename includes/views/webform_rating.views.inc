<?php

/**
 * @file
 *  Views integration of the webform rating module.
 */

/**
 * Implements hook_views_data().
 */
function webform_rating_views_data() {
  $data = array();

  $data['webform_rating_nodes']['table']['group']  = t('Webform Rating');

  $data['webform_rating_nodes']['table']['base'] = array(
    'field' => 'sid',
    'title' => t('Webform rating'),
    'help' => t('Webform rating'),
  );

  // The webform submission id.
  $data['webform_rating_nodes']['sid'] = array(
    'title' => t('Rating submission id'),
    'help' => t('The rating submission id.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'base' => 'webform_submissions', // The name of the table to join with.
      'base field' => 'sid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Referenced Webform Submissions'),
    ),
  );

  // The target node id.
  $data['webform_rating_nodes']['target_nid'] = array(
    'title' => t('Target node'),
    'help' => t('Target node'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'base' => 'node', // The name of the table to join with.
      'base field' => 'nid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Referenced target'),
    ),
  );

  // The number of answered questions.
  $data['webform_rating_nodes']['questions_answered'] = array(
    'title' => t('Questions answered'),
    'help' => t('Questions answered'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // The rating.
  $data['webform_rating_nodes']['rating'] = array(
    'title' => t('Rating'),
    'help' => t('Rating'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['webform_rating_nodes']['table']['join'] = array(
    'webform_submissions' => array(
      'left_field' => 'sid',
      'field' => 'sid',
    ),
    'node' => array(
      'left_field' => 'target_nid',
      'field' => 'nid',
    ),
  );

  return $data;
}
