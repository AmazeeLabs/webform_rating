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
  'category' => array(t('Custom'), -9),
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
    if (!empty($context[1]->data)) {
      $webform_nid = $context[1]->data->nid;
    }
    else {
      $webform_nid = variable_get('webform_rating_default_form', FALSE);
    }
    if (!empty($target_nid) && !empty($webform_nid)) {
      $webform = node_load($webform_nid);
      if (WebformRating::checkTargetRatingAccess($target_nid, $webform) === TRUE) {
        $block->content = l(t('Rate it'), 'node/' . $target_nid . '/rate/' . $webform_nid);
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
