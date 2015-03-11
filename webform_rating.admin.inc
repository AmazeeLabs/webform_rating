<?php

/**
 * Administration menu callbacks.
 */

/**
 * Form constructor to recalculate the webform scores.
 *
 * @todo: We should also provide the option to run the calculation in background,
 * using the queue api.
 */
function webform_rating_recalculate_form($form, &$form_state, $webform) {
  $form['description'] = array(
    '#type' => 'item',
    '#markup' => t('The recalculation of the scores may take some more time, depending on how many submissions exists for this form.')
  );
  $form['webform_nid'] = array(
    '#type' => 'value',
    '#value' => $webform->nid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Submit handler for the scores recalculation.
 */
function webform_rating_recalculate_form_submit(&$form, &$form_state) {
  // @todo: We should implement the recalculation of the scores in background,
  // using the queue API.
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  $submissions = webform_get_submissions(array('nid' => $form_state['values']['webform_nid']));
  foreach ($submissions as $submission) {
    $rating = webform_rating_load_by_sid($submission->sid);
    if (!empty($rating)) {
      WebformRating::updateRatingFromSubmission($rating->target_nid, $submission);
    }
  }
  drupal_set_message(t('The scores have been updated.'));
}
