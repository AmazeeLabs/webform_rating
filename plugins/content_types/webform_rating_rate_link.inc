<?php

/**
 * @file
 * Content type that displays a rate link.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Webform rating: rate link'),
  'content_types' => 'webform_rating_rate_link',
  'single' => TRUE,
  'render callback' => 'webform_rating_rate_link_render',
  'required context' => array(new ctools_context_required(t('Target Content'), 'node'), new ctools_context_optional(t('Webform'), 'node')),
  'category' => array(t('Webform'), -9),
  'edit form' => 'webform_rating_rate_link_settings_form',
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
function webform_rating_rate_link_render($subtype, $conf, $args, $context) {
  $block = new stdClass();
  if (!empty($context[0]->data)) {
    $target_nid = $context[0]->data->nid;
    $webform_nid = webform_rating_get_webform_nid_from_optional_context($context[1], $context[0]->data);
    if (!empty($target_nid) && !empty($webform_nid)) {
      $webform = node_load($webform_nid);
      if (WebformRating::checkTargetRatingAccess($target_nid, $webform) === TRUE) {
        global $user;
        $total_components = WebformRating::getRatingComponents($webform->webform['components'], FALSE);
        $rating = webform_rating_load_by_conditions(array('uid' => $user->uid, 'target_nid' => $target_nid, 'webform_nid' => $webform_nid));
        if (!empty($rating)) {
          $rating = reset($rating);
        }
        $classes = array('call-to-action');
        // If the user completed the rating, then use a different title for the
        // link and add an additional class.
        if (!empty($rating) && count($total_components) == $rating->questions_answered) {
          $classes[] = 'rating-completed';
          $link_title = t('Edit');
        }
        else {
          $link_title = t('Rate it');
        }
        $block->content = l($link_title, 'node/' . $target_nid . '/rate/' . $webform_nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => $classes)));
      }
    }
  }

  return $block;
}

/**
 * Configuration form, not used at the moment.
 */
function webform_rating_rate_link_settings_form($form, &$form_state) {
  return $form;
}
