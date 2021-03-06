<?php

/**
 * @file
 *  Utility class for the Webform Rating module.
 */

/**
 * A utility class with static methods for the Webform Rating module.
 */
class WebformRating {

  /**
   * Computes the score for a webform submission.
   *
   * @param object $submission
   *  A webform submission object.
   *
   * @return double|0
   *  The score of the submission if the webform could be loaded, 0 otherwise.
   */
  public static function getScore($submission) {
    $webform = node_load($submission->nid);
    // If no webform found, we reset the score to 0. This should never actually
    // happen.
    if (empty($webform)) {
      return 0;
    }

    // Get only the rating components out of the total amount of webform
    // components.
    $rating_components = WebformRating::getRatingComponents($webform->webform['components']);

    // Again, if no rating components, we can just return 0.
    if (empty($rating_components)) {
      return 0;
    }

    // Get the raw points per component and the weight for each component.
    $points_per_component = WebformRating::getPointsPerComponent($submission, $rating_components);
    $total_points = array_sum($points_per_component);

    return $total_points;
  }

  /**
   * Returns the components which should act for rating components from a set of
   * webform components.
   *
   * @param array $webform_components
   *  An array with all the webform components. Typically, it is the components
   *  array from the webform node ($webform->webform['components']);
   *
   * @param int $include_not_count
   *  If we should include the components that should not count for the total
   *  amount of rating components.
   *
   * @return array
   *  A subset of the $webform_components which should be taken into
   *  consideration when calculating the rating.
   */
  public static function getRatingComponents($webform_components, $include_not_count = TRUE) {
    $rating_components = array();
    foreach ($webform_components as $key => $component) {
      // We consider as rating components all the components which are 'select'
      // and have rating setting for the items. Also, we check that the
      // rating_component checkbox is not empty.
      if ($component['type'] == 'select' && !empty($component['extra']['items_rating']) && !empty($component['extra']['rating_component'])) {
        // We count the component if either we include the components event if
        // they should not count for the total, or if the component does
        // actually count for the total.
        if (!empty($include_not_count) || !empty($component['extra']['rating_component_count'])) {
          $rating_components[$key] = $component;
        }
      }
    }
    return $rating_components;
  }

  /**
   * Calculates the points for each component for a subsmission.
   *
   * @param object $submission
   *  A webform submission.
   *
   * @param array $rating_components
   *  An array with all the rating components. Typically, it is the components
   *  array from the WebformRating::getRatingComponents().
   *
   *  @return array
   *   An array with the points for each component.
   */
  public static function getPointsPerComponent($submission, $rating_components) {
    $points_per_component = array();
    foreach ($submission->data as $component_id => $answer_values) {
      if (empty($rating_components[$component_id])) {
        continue;
      }
      $component_score = 0;
      $items_rating = $rating_components[$component_id]['extra']['items_rating'];
      foreach ($answer_values as $option_key) {
        $component_score += !empty($items_rating[$option_key]) ? $items_rating[$option_key] : 0;
      }
      $points_per_component[$component_id] = $component_score;
    }

    return $points_per_component;
  }

  /**
   * Returns the set of components which were filled in.
   *
   * @param object $submission
   *  The webform submission object.
   *
   * @param array $rating_components
   *  An array with all the rating components. Typically, it is the components
   *  array from the WebformRating::getRatingComponents().
   *
   * @return array
   *  An subset of the $rating_components which have submitted values.
   */
  public static function getFilledComponents($submission, $rating_components) {
    $filled_in_components = array();
    foreach ($rating_components as $component_id => $component) {
      // A component is filled in if it has at least one value in the submission
      // and that value is not the empty string.
      // Although in our case, we should always have arrays (because we handle
      // only select elements) make sure that we have an array here.
      if (!empty($submission->data[$component_id]) && is_array($submission->data[$component_id])) {
        $is_empty = TRUE;
        foreach ($submission->data[$component_id] as $value) {
          if ($value !== '' && !is_null($value)) {
            $is_empty = FALSE;
            break;
          }
        }
        if (!$is_empty) {
          $filled_in_components[$component_id] = $component;
        }
      }
    }
    return $filled_in_components;
  }

  /**
   * Updates the rating of a target node for a submission.
   *
   * @param object $submission
   *  The webform submission.
   *
   * @param int $target_nid
   *  The id of the target node.
   *
   * @return object|FALSE
   *  If the operation succeded, it will return the rating object, otherwise it
   *  will return FALSE.
   */
  public static function updateRatingFromSubmission($target_nid, $submission) {
    $webform_node = node_load($submission->nid);
    $rating = webform_rating_load_by_sid($submission->sid);
    if (empty($rating)) {
      $rating = new stdClass();
      $rating->sid = $submission->sid;
      $rating->is_new = TRUE;
    }
    $rating->uid = $submission->uid;
    $rating->webform_nid = $submission->nid;
    $rating->target_nid = $target_nid;
    $rating->rating = WebformRating::getScore($submission);
    $rating_components = WebformRating::getRatingComponents($webform_node->webform['components'], FALSE);
    $rating->questions_answered = count(WebformRating::getFilledComponents($submission, $rating_components));
    $rating->total_questions = count($rating_components);

    // And save the rating.
    $update = webform_rating_save_rating($rating);
    if ($update !== FALSE) {
      module_invoke_all('webform_rating_update', $rating);
    }
    return $update;
  }

  /**
   * Checks if a target can be rated by an account using a specific webform type.
   *
   * @param int $target_nid
   *  The target node id to check.
   *
   * @param string $webform_node
   *  The webform node for which to check the access.
   *
   * @param object $account
   *  Optional, the user account for which to check the access. By default is
   *  the logged in user.
   *
   * @return array $result|TRUE
   *  TRUE if the user has access, or an array with a error message if the user
   *  does not have access.
   */
  public static function checkTargetRatingAccess($target_nid, $webform_node, $account = NULL) {
    // Check that the target node is valid.
    $target_node = node_load($target_nid);
    if (empty($target_node)) {
      return array('access' => FALSE, 'error_message' => t('The target content for the rating could not be loaded. Please contact the site administrator.'));
    }

    // Check if we can rate this type of nodes using the type of the webform;
    if (!webform_rating_can_rate_node_type($webform_node->type, $target_node->type)) {
      return array('access' => FALSE, 'error_message' => t('You are not allowed to rate this type of content. Please contact the site administrator.'));
    }

    // The last check: make sure that the user has access to rate the target.
    if (!webform_rating_user_has_rating_access($target_node, $webform_node, $account)) {
      return array('access' => FALSE, 'error_message' => t('You are not allowed to rate the content.'));
    }

    // If we are here, the user is allowed to rate.
    return TRUE;
  }

  /**
   * Returns an array with all the rating webforms on the site.
   *
   * The return array is an associtive array of node ids and node titles.
   */
  public static function getAllRatingWebforms() {
    $query = db_select('webform', 'w');
    $query->innerJoin('node', 'n', 'n.nid = w.nid');
    $query->fields('n', array('nid', 'title', 'type'));
    $query->condition('n.status', NODE_PUBLISHED);
    $all_webforms = $query->execute()->fetchAll();
    // From the set of all webform, we have to remove the ones for which the
    // rating functionality is not enabled.
    $rating_webforms = array();
    foreach ($all_webforms as $nid => $row) {
      if (!variable_get('webform_rating_node_' . $row->type, FALSE)) {
        unset($all_webforms[$nid]);
        continue;
      }
      $rating_webforms[$row->nid] = $row->title;
    }
    return $rating_webforms;
  }
}
