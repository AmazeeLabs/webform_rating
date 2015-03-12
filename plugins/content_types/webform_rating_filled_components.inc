<?php

/**
 * @file
 * Content type that displays how many components are filled for a rating form.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Webform rating: filled components'),
  'content_types' => 'webform_rating_filled_components',
  'single' => TRUE,
  'render callback' => 'webform_rating_filled_components_render',
  'required context' => array(new ctools_context_required(t('Target Content'), 'node'), new ctools_context_required(t('Account'), 'user'), new ctools_context_optional(t('Webform'), 'node')),
  'category' => array(t('Webform'), -9),
  'edit form' => 'webform_rating_filled_components_settings_form',
);

/**
 * Run-time rendering of the body of the block.
 *
 * @param $subtype
 * @param $conf
 *   Configuration as done at admin time
 * @param $args
 * @param $context
 *   Context - in this case we don't have any
 *
 * @return
 *   An object with at least title and content members
 */
function webform_rating_filled_components_render($subtype, $conf, $args, $context) {
  $block = new stdClass();
  if (!empty($context[0]->data) && !empty($context[1]->data)) {
    $target_nid = $context[0]->data->nid;
    $account_uid = $context[1]->data->uid;
    $webform_nid = webform_rating_get_webform_nid_from_optional_context($context[2]);
    if (!empty($target_nid) && !empty($account_uid) && !empty($webform_nid)) {
      // Get all the total rating components, so that we can calculate the
      // total number of them.
      $webform = node_load($webform_nid);
      $total_components = WebformRating::getRatingComponents($webform->webform['components']);
      // Search for a rating.
      $rating = webform_rating_load_by_conditions(array('uid' => $account_uid, 'target_nid' => $target_nid, 'webform_nid' => $webform_nid));
      $filled_in = 0;
      if (!empty($rating)) {
        // If we have a rating, get the number of 'answered questions', which is
        // the number of filled in components (fields).
        $rating = reset($rating);
        $filled_in = $rating->questions_answered;
      }
      $block->content = t('!filled_in_components out of !total_components fields filled in.', array('!filled_in_components' => $filled_in, '!total_components' => count($total_components)));
    }
  }

  return $block;
}

/**
 * Configuration form, not used at the moment.
 */
function webform_rating_filled_components_settings_form($form, &$form_state) {
  return $form;
}
