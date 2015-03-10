<?php

/**
 * @file
 *  Class with static methods acting as helpers for the webform client forms.
 *
 */

/**
 * A class with static helper methods for the webform client forms.
 */
class WebformRatingClientForm {

  /**
   * Helper method for the hook_form_FORM_ID_alter() for webform_client_form()
   */
  public static function webformClientFormAlter(&$form, &$form_state) {
    // Try to get the target node id and if it is empty, we do not show the
    // form.
    $target_nid = WebformRatingClientForm::getTargetNidFromContext($form);
    if (empty($target_nid)) {
      $form['#access'] = FALSE;
      drupal_set_message(t('The target content id for the rating could not be determined. Please contact the site administrator.'), 'warning');
      return;
    }

    // Also, check that the user can really rate the target.
    $rating_access = WebformRatingClientForm::checkTargetRatingAccess($target_nid, $form['#node']->type);
    if ($rating_access !== TRUE) {
      $form['#access'] = FALSE;
      if (!empty($rating_access['error_message'])) {
        drupal_set_message($rating_access['error_message'], 'warning');
      }
      return;
    }

    // If we are here, the user can rate and we can put the target node id in
    // the form.
    $form['rating_target_nid'] = array(
      '#type' => 'value',
      '#value' => $target_nid,
    );
    // Add our submit handler to the form.
    $form['#submit'][] = 'webform_rating_webform_client_form_submit';
  }

  /**
   * Helper method for the webform_rating_webform_client_form_submit().
   */
  public static function webformClientFormSubmit(&$form, &$form_state) {
    if (!empty($form_state['values']['details']['sid'])) {
      $submission = webform_menu_submission_load($form_state['values']['details']['sid'], $form_state['values']['details']['nid']);
      $rating = webform_rating_load_by_sid($submission->sid);
      if (empty($rating)) {
        $rating = new stdClass();
        $rating->sid = $submission->sid;
        $rating->is_new = TRUE;
      }
      $rating->webform_nid = $form_state['values']['details']['nid'];
      $rating->target_nid = $form_state['values']['rating_target_nid'];
      // @todo: Compute these values.
      $rating->questions_answered = 0;
      $rating->rating = 1;

      webform_rating_save_rating($rating);
    }
  }

  /**
   * Returns the target nid for a rating. It tries to load it from the webform
   * subsmission from the form, if exists, otherwise it will search for it
   * in the global $_REQUEST.
   *
   * @param array $form
   *  A entire form, usually this is a webform_client_form().
   *
   * @return int|NULL
   *  The target node id if found, NULL otherwise.
   */
  public static function getTargetNidFromContext(&$form) {
    $target_nid = NULL;
    // Search first in the global $_REQUEST.
    if (!empty($_REQUEST['rating_' . $form['#node']->nid])) {
      $target_nid = $_REQUEST['rating_' . $form['#node']->nid];
    }

    // If we have a submission, then we overwrite the target nid if possible.
    if (!empty($form['details']['sid']['#value'])) {
      $rating = webform_rating_load_by_sid($form['details']['sid']['#value']);
      if (!empty($rating)) {
        $target_nid = $rating->target_nid;
      }
    }
    return $target_nid;
  }

  /**
   * Checks if a target can be rated by an account using a specific webform type.
   *
   * @param int $target_nid
   *  The target node id to check.
   *
   * @param string $webform_node_type
   *  The webform node type for which to check the access.
   *
   * @param object $account
   *  Optional, the user account for which to check the access. By default is
   *  the logged in user.
   *
   * @return array $result|TRUE
   *  TRUE if the user has access, or an array with a error message if the user
   *  does not have access.
   */
  public static function checkTargetRatingAccess($target_nid, $webform_node_type, $account = NULL) {
    // Check that the target node is valid.
    $target_node = node_load($target_nid);
    if (empty($target_node)) {
      return array('access' => FALSE, 'error_message' => t('The target content for the rating could not be loaded. Please contact the site administrator.'));
    }

    // Check if we can rate this type of nodes using the type of the webform;
    if (!webform_rating_can_rate_node_type($webform_node_type, $target_node->type)) {
      return array('access' => FALSE, 'error_message' => t('You are not allowed to rate this type of content. Please contact the site administrator.'));
    }

    // The last check: make sure that the user has access to rate the target.
    if (!webform_rating_user_has_rating_access($target_node, $account)) {
      return array('access' => FALSE, 'error_message' => t('You are not allowed to rate the content.'));
    }

    // If we are here, the user is allowed to rate.
    return TRUE;
  }
}
