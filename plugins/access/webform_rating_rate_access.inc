<?php

/**
 * @file
 * Plugin to provide access control/visibility based on length of
 * simplecontext argument (in URL).
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Webform rating: rate access"),
  'description' => t('Control access by checking if the user can rate a content.'),
  'callback' => 'webform_rating_rate_access_check',
  'settings form' => 'webform_rating_rate_access_check_settings',
  'summary' => 'webform_rating_rate_access_check_summary',
  'required context' => array(new ctools_context_required(t('Target Content'), 'node'), new ctools_context_optional(t('Webform'), 'node')),
);

/**
 * Settings form.
 *
 * Not used at the moment.
 */
function webform_rating_rate_access_check_settings(&$form, &$form_state, $conf) {
  return $form;
}

/**
 * Check for access.
 */
function webform_rating_rate_access_check($conf, $context) {
  $access = FALSE;
  if (!empty($context[0]->data)) {
    $target_nid = $context[0]->data->nid;
    $webform_nid = webform_rating_get_webform_nid_from_optional_context($context[1]);
    if (!empty($target_nid) && !empty($webform_nid)) {
      $webform = node_load($webform_nid);
      if (WebformRating::checkTargetRatingAccess($target_nid, $webform) === TRUE) {
        $access = TRUE;
      }
    }
  }
  return $access;
}

/**
 * Provide a summary description based upon the checked roles.
 */
function webform_rating_rate_access_check_summary($conf, $context) {
  if ($context[1]->type == 'any') {
    return t('User can rate "@target" using the default rating form', array('@target' => $context[0]->identifier));
  }
  return t('User can rate "@target" using the form in "@webform"', array('@target' => $context[0]->identifier, '@webform' => $context[1]->identifier));
}
