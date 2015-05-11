<?php

/**
 * @file
 * Definition of webform_rating_handler_rate_status.
 */

/**
 * Field handler to present a Rated / Unrated status.
 *
 * @ingroup views_field_handlers
 */
class webform_rating_handler_rate_status extends views_handler_field {

  /**
   * (non-PHPdoc)
   * @see views_handler_field::query()
   */
  public function query() {
    // We just add a field that we later can check on render to see if it empty
    // or not.
    $this->add_additional_fields(array('rating_sid' => array('table' => 'webform_rating_nodes', 'field' => 'sid')));
  }

  /**
   * (non-PHPdoc)
   * @see views_handler_field::render()
   */
  function render($values) {
    // We just print a Rated/Unrated string.
    $field_alias = $this->aliases['rating_sid'];
    if (!empty($values->{$field_alias})) {
      return '<div class="rated">' . t('Rated') . '</div>';
    }
    else {
      return '<div class="unrated">' . t('Unrated') . '</div>';
    }
  }
}
