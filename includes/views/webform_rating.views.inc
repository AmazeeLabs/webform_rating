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

  $data['webform_rating_nodes']['uid'] = array(
    'title' => t('Rating user id'),
    'help' => t('The author of the rating.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_in_operator',
      'options callback' => 'webform_rating_rating_author_options',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
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

  // The number of total questions.
  $data['webform_rating_nodes']['total_questions'] = array(
    'title' => t('Total questions'),
    'help' => t('Total questions'),
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

  // Link to rate.
  $data['webform_rating_nodes']['table']['entity type'] = 'node';
  $data['webform_rating_nodes']['rate_link'] = array(
    'field' => array(
      'title' => t('Rate link'),
      'help' => t('Provide a simple link to rate a node.'),
      'handler' => 'webform_rating_handler_rate_link',
    ),
  );

  // Rating status.
  $data['webform_rating_nodes']['rate_status'] = array(
    'field' => array(
      'title' => t('Rate status'),
      'help' => t('Shows a rated/unrated string, depending if the user rated.'),
      'handler' => 'webform_rating_handler_rate_status',
    ),
  );

  $data['webform_rating_nodes']['table']['join'] = array(
    'webform_submissions' => array(
      'left_field' => 'sid',
      'field' => 'sid',
    ),
  );

  return $data;
}

function webform_rating_views_data_alter(&$data) {
  // Joins
  $data['node']['table']['join']['webform_rating_nodes'] = array(
    'left_field' => 'target_nid',
    'field' => 'nid',
  );
  // The webform rating relationship.
  $data['node']['rating_submission'] = array(
    'group' => t('Rating submission'),
    'title' => t('Rating submission'),
    'relationship' => array(
      'title' => t('Rating submission'),
      'help' => t('The rating of the submission.'),
      'base' => 'webform_rating_nodes',
      'base field' => 'target_nid',
      'relationship table' => 'node',
      'relationship field' => 'nid',
      'handler' => 'views_handler_relationship_webform_rating',
      'label' => t('Rating submission'),
    ),
  );
}
