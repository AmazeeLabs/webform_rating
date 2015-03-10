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
  'required context' => array(new ctools_context_required(t('Target Content'), 'node'), new ctools_context_required(t('Webform'), 'node')),
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
  return FALSE;
}

/**
 * Provide a summary description based upon the checked roles.
 */
function webform_rating_rate_access_check_summary($conf, $context) {
  return 'Some summary';
}
