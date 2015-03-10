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
   * Helper method for the webform_rating_client_form().
   */
  public static function webformClientForm($form, &$form_state, $node, $submission = FALSE, $is_draft = FALSE, $filter = TRUE) {
    global $user;
    // We still want to keep the old base form id, otherwise no form alters will
    // work anymore.
    $form_state['build_info']['base_form_id'] = 'webform_client_form';
    if (webform_rating_is_enabled($node->type)) {
      $context = array('webform' => $node);
      if (!empty($submission)) {
        $context['submission'] = $submission;
      }
      $target_nid = WebformRatingClientForm::getTargetNidFromContext($context);

      // @todo: Maybe think to some other better solution to hide the entire
      // form.
      if (empty($target_nid)) {
        $form['#access'] = FALSE;
        drupal_set_message(t('The target content id for the rating could not be determined. Please contact the site administrator.'), 'warning');
        return;
      }

      // Also, check that the user can really rate the target.
      $rating_access = WebformRatingClientForm::checkTargetRatingAccess($target_nid, $node->type);
      if ($rating_access !== TRUE) {
        $form['#access'] = FALSE;
        if (!empty($rating_access['error_message'])) {
          drupal_set_message($rating_access['error_message'], 'warning');
        }
        return;
      }

      // Try to see if the user already has a rating for this target. If yes, we
      // will just load that submission.
      $ratings = webform_rating_load_by_conditions(array('uid' => !empty($submission) ? $submission->uid : $user->uid, 'target_nid' => $target_nid, 'webform_nid' => $node->nid));
      if (!empty($ratings)) {
        $rating = reset($ratings);
        $submission = webform_menu_submission_load($rating->sid, $node->nid);
      }
      // If we are here, even if we have a submission, we force to a new one,
      // because it means we want to submit the same webform but for another
      // target. And by default, the webform module will load the draft
      // submission if an user has one, preventing an user to have more drafts
      // for the same webform. In our case, it is OK to have.
      else {
        $submission = FALSE;
      }

      // And now just load the default form and then add our fields.
      $form = webform_client_form($form, $form_state, $node, $submission, $is_draft, $filter);

      $form['rating_target_nid'] = array(
        '#type' => 'value',
        '#value' => $target_nid,
      );
      // Add our submit handler to the form.
      $form['#submit'][] = 'webform_rating_webform_client_form_submit';

      return $form;
    }
    else {
      return webform_client_form($form, $form_state, $node, $submission, $is_draft, $filter);
    }
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
        $rating->uid = $submission->uid;
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
   * @param array $context
   *  A an array which can contain a 'webform' node and/or a 'submission'. In
   *  case of a submision it will try to load the data from the
   *  webform_rating_nodes table.
   *
   * @return int|NULL
   *  The target node id if found, NULL otherwise.
   */
  public static function getTargetNidFromContext($context = array()) {
    $target_nid = NULL;

    // If we have the target in the request, we return it directly.
    if (!empty($context['webform']) && !empty($_REQUEST['rating_' . $context['webform']->nid])) {
      return $_REQUEST['rating_' . $context['webform']->nid];
    }

    // Search for a target node id in the submission, if available.
    if (!empty($context['submission'])) {
      $rating = webform_rating_load_by_sid($context['submission']->sid);
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
