<?php

/**
 * @file
 * Definition of views_handler_relationship_webform_rating.
 */

class views_handler_relationship_webform_rating extends views_handler_relationship {

  function option_definition() {
    $options = parent::option_definition();
    $options['user'] = array('default' => 'all');
    $options['webform'] = array('default' => 'all');

    return $options;
  }

  /**
   * Add a user selector.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $user_options = array(
      'all' => t('Any user'),
      'current' => t('Current logged in user'),
    );
    $form['user'] = array(
      '#type' => 'select',
      '#options' => $user_options,
      '#default_value' => $this->options['user'],
      '#title' => t('Filter rating by user'),
      '#description' => t('You can choose if you want to filter the rating by the current logged in user, or to consider the ratings of all the users.'),
    );
    $webform_options = array(
      'all' => t('Any webform'),
      'current' => t('Current rating webform'),
    );
    $form['webform'] = array(
      '#type' => 'select',
      '#options' => $webform_options,
      '#default_value' => $this->options['webform'],
      '#title' => t('Filter rating by current webform'),
      '#description' => t('You can choose if you want to filter the rating by the current webform which will be loaded from the context.'),
    );
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    // Figure out what base table this relationship brings to the party.
    $table_data = views_fetch_data($this->definition['base']);
    $base_field = empty($this->definition['base field']) ? $table_data['table']['base']['field'] : $this->definition['base field'];

    $this->ensure_my_table();

    $def = $this->definition;
    $def['table'] = $this->definition['base'];
    $def['field'] = $base_field;
    $def['left_table'] = $this->table_alias;
    $def['left_field'] = $this->field;
    if (!empty($this->options['required'])) {
      $def['type'] = 'INNER';
    }

    $def['extra'] = array();
    if ($this->options['user'] == 'current') {
      $def['extra'][] = array(
        'field' => 'uid',
        'value' => '***CURRENT_USER***',
      );
    }

    if ($this->options['webform'] == 'current') {
      // @todo: we should do a query substitution if we use it somewhere else.
      $webform_nid = webform_rating_get_webform_nid_from_optional_context();
      if (!empty($webform_nid)) {
        $def['extra'][] = array(
          'field' => 'webform_nid',
          'value' => $webform_nid
        );
      }
    }

    if (!empty($def['join_handler']) && class_exists($def['join_handler'])) {
      $join = new $def['join_handler'];
    }
    else {
      $join = new views_join();
    }

    $join->definition = $def;
    $join->construct();
    $join->adjusted = TRUE;

    // use a short alias for this:
    $alias = $def['table'] . '_' . $this->table;

    $this->alias = $this->query->add_relationship($alias, $join, $this->definition['base'], $this->relationship);
  }
}
