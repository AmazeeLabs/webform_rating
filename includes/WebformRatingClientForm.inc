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
      $rating_access = WebformRating::checkTargetRatingAccess($target_nid, $node);
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
      $target_node = node_load($target_nid);
      $form['submitted']['target_node_title'] = array(
        '#type' => 'item',
        '#markup' => t('Input your rating for !node_title', array('!node_title' => l($target_node->title, 'node/' . $target_node->nid))),
        '#weight' => -50,
      );
      // Add our submit handler to the form.
      $form['#submit'][] = 'webform_rating_webform_client_form_submit';
      if (!empty($form['actions']['submit'])) {
        $form['actions']['submit']['#value'] = t('Save');
      }

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
    if (empty($form_state['save_draft']) && empty($form_state['webform_completed'])) {
      return;
    }
    if (!empty($form_state['values']['details']['sid'])) {
      $submission = webform_menu_submission_load($form_state['values']['details']['sid'], $form_state['values']['details']['nid']);
      WebformRating::updateRatingFromSubmission($form_state['values']['rating_target_nid'], $submission);
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

    // We also try to load the target if we are on a page like: node/nid/rate.
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == 'rate') {
      return arg(1);
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

}
