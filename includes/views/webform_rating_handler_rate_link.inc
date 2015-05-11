<?php

/**
 * @file
 * Definition of webform_rating_handler_rate_link.
 */

/**
 * Field handler to present a link for rating.
 *
 * @ingroup views_field_handlers
 */
class webform_rating_handler_rate_link extends views_handler_field {

  function construct() {
    parent::construct();
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => '', 'translatable' => TRUE);
    $options['destination'] = array('default' => FALSE, 'bool' => TRUE);
    return $options;
  }

  public function query() {
  }

  function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    $form['destination'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use destination'),
      '#description' => t('Add destination to the link'),
      '#default_value' => $this->options['destination'],
      '#fieldset' => 'more',
    );
    parent::options_form($form, $form_state);
  }

  function render($values) {
    // @todo: we have to find a better solution for this.
    if (!empty($values->nid)) {
      $node = node_load($values->nid);
      return $this->render_link($node, $values);
    }
  }

  function render_link($node, $values) {
    $webform_nid = webform_rating_get_webform_nid_from_optional_context(NULL, $node);
    if (!empty($webform_nid)) {
      $webform = node_load($webform_nid);
      if (WebformRating::checkTargetRatingAccess($node->nid, $webform) === TRUE) {
        $this->options['alter']['make_link'] = TRUE;
        $text = !empty($this->options['text']) ? $this->options['text'] : t('Rate');

        if (!empty($this->options['destination'])) {
          $this->options['alter']['query'] = drupal_get_destination();
        }

        $this->options['alter']['path'] = "node/" . $node->nid . "/rate/" . $webform_nid;
        return $text;
      }
    }
  }
}
