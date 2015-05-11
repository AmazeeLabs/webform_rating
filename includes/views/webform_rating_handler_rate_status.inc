<?php

/**
 * @file
 * Definition of webform_rating_handler_rate_status.
 */

/**
 * Field handler to present a Complete / Incomplete status.
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
    $this->add_additional_fields(array('rating_sid' => array('table' => 'webform_rating_nodes', 'field' => 'sid'),
                                       'rating_total_questions' => array('table' => 'webform_rating_nodes', 'field' => 'total_questions'),
                                       'rating_questions_answered' => array('table' => 'webform_rating_nodes', 'field' => 'questions_answered')));
  }

  /**
   * (non-PHPdoc)
   * @see views_handler_field::render()
   */
  function render($values) {
    // We just print a Complete/Incomplete string.
    $rating_sid_alias = $this->aliases['rating_sid'];
    $rating_total_questions_alias = $this->aliases['rating_total_questions'];
    $rating_questions_answered_alias = $this->aliases['rating_questions_answered'];
    if (!empty($values->{$rating_sid_alias}) && $values->{$rating_total_questions_alias} == $values->{$rating_questions_answered_alias}) {
      return '<div class="rating-complete">' . t('Complete') . '</div>';
    }
    else {
      return '<div class="rating-incomplete">' . t('Incomplete') . '</div>';
    }
  }
}
