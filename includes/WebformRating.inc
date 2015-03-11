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
    $components_weight = WebformRating::getComponentsWeight($rating_components);

    // Compute the weigthed average based on theese values.
    $total_weight = array_sum($components_weight);
    // Avoid a 'Division by zero' warning. This can happen when all the weights
    // of all the components are specifically set to 0.
    if (empty($total_weight)) {
      return 0;
    }
    $total_points = 0;
    foreach ($points_per_component as $component_id => $points) {
      $total_points += $points * $components_weight[$component_id];
    }

    // And finally compute the total score.
    $final_score = round($total_points / $total_weight, 2);
    return $final_score;
  }

  /**
   * Returns the components which should act for rating components from a set of
   * webform components.
   *
   * @param array $webform_components
   *  An array with all the webform components. Typically, it is the components
   *  array from the webform node ($webform->webform['components']);
   *
   * @return array
   *  A subset of the $webform_components which should be taken into
   *  consideration when calculating the rating.
   */
  public static function getRatingComponents($webform_components) {
    $rating_components = array();
    foreach ($webform_components as $key => $component) {
      // We consider as rating components all the components which are 'select'
      // and have rating setting for the items. This may change in the future.
      if ($component['type'] == 'select' && !empty($component['extra']['items_rating'])) {
        $rating_components[$key] = $component;
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
   * Calculates the weight of each component in a set of webform components.
   *
   * @param array $rating_components
   *  An array with all the rating components. Typically, it is the components
   *  array from the WebformRating::getRatingComponents().
   *
   * @return array
   *  An array with the weight of each component (it is a percentage between 0
   *  and 100). If no component has a weight, then all the components will have
   *  the same weight (it doesn't matter what weight, it will not affect the
   *  total score, so we will just put 1).
   */
  public static function getComponentsWeight($rating_components) {
    $components_weight = array();
    $used_weight = 0;
    foreach ($rating_components as $component_id => $component) {
      // We specifically check for empty string, because the value '0' is a
      // valid weight!
      if (isset($component['extra']['component_weight']) && $component['extra']['component_weight'] !== '') {
        $used_weight += $component['extra']['component_weight'];
        $components_weight[$component_id] = $component['extra']['component_weight'];
      }
    }
    // We split the remaining weight, but only if we have any special weights.
    if (!empty($components_weight)) {
      $left_weight = 100 - $used_weight;
      // The left weight must be positive, otherwise something is wrong, and we
      // log this.
      // @todo: should we also send an e-mail?
      // @todo: also, should we reset the weights in this case? At the moment
      // they are returned like that, even if the total sum is bigger than 100.
      if ($left_weight < 0) {
        $first_component = reset($rating_components);
        $webform_node = node_load($first_component['nid']);
        watchdog('webform rating', 'The total weight used by the "@webform_title" webform components is bigger than 100%. The current value is: !current_value%',
                  array('@webform_title' => $webform_node->title, '!current_value' => $used_weight), WATCHDOG_ERROR, url('node/' . $webform_node->nid, array('absolute' => TRUE)));
        $left_weight = 0;
      }
      // And split the weight between the remaining rating components.
      if (count($rating_components) - count($components_weight) > 0) {
        $split_weight = $left_weight / (count($rating_components) - count($components_weight));
        foreach ($rating_components as $component_id => $component) {
          if (!isset($components_weight[$component_id])) {
            $components_weight[$component_id] = $split_weight;
          }
        }
      }
    }
    else {
      // If no sepcial weight, we just set it to '1', as it will not affect the
      // total score.
      foreach ($rating_components as $component_id => $component) {
        $components_weight[$component_id] = 1;
      }
    }

    return $components_weight;
  }
}