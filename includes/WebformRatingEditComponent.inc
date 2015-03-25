<?php

/**
 * @file
 *  A class with some helper methods for the edit webform component form.
 */

/**
 * A class with some static methods that are helpers for the editing webform
 * components.
 *
 */
class WebformRatingEditComponent {

  /**
   * Helper for the hook_form_FORM_ID_alter() for webform_component_edit_form()
   */
  public static function webformComponentEditFormAlter(&$form, &$form_state) {
    // We do not support yet the options_element module. We should check if this
    // is hard to implement. If the options_element module is enabled, the form
    // is a bit different, see _webform_edit_select().
    if (module_exists('options_element')) {
      drupal_set_message(t('The rating feature is not working at the moment when the options_element module is enabled.'), 'warning');
      return;
    }
    // Get the items value which represent the available options for the select.
    $items_value = !empty($form['extra']['items']['#default_value']) ? $form['extra']['items']['#default_value'] : '';
    // If we have however a value in the $form_state, we use that, because it
    // means it was submitted with ajax.
    if (isset($form_state['values']['extra']['items'])) {
      $items_value = $form_state['values']['extra']['items'];
    }

    // Make sure the select.inc is available also for ajax requests.
    form_load_include($form_state, 'inc', 'webform', 'components/select');
    // Get an array of options out of the formatted string.
    $options = _webform_select_options_from_text($items_value, TRUE);
    $weight = $form['extra']['items']['#weight'];
    $form['extra']['items_rating'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id="items_rating">',
      '#suffix' => '</div>',
      '#title' => t('Rating points')
    );
    if (!empty($options)) {
      $component = $form_state['build_info']['args'][1];
      foreach ($options as $key => $value) {
        $weight += 0.01;
        $field_title = t('Points for %option: ', array('%option' => $value));
        $form['extra']['items_rating'][$key] = array(
          '#type' => 'textfield',
          '#title' => $field_title,
          '#default_value' => isset($component['extra']['items_rating'][$key]) ? $component['extra']['items_rating'][$key] : 0,
          '#field_prefix' => $field_title,
          '#size' => 3,
          '#field_suffix' => t('points'),
          '#title_display' => 'invisible',
          '#weight' => $weight,
        );
      }
    }

    // Add the checkbox to set if the component is a rating component or not.
    $form['extra']['rating_component'] = array(
      '#type' => 'checkbox',
      '#title' => t('Is this a rating form component?'),
      '#description' => t('If this is checked, this form element will count as a rating element.'),
      '#default_value' => isset($component['extra']['rating_component']) ? $component['extra']['rating_component'] : TRUE,
    );

    // Add the ajax behavior on the items textarea.
    $form['extra']['items']['#ajax'] = array(
      'callback' => 'webform_rating_webform_component_items_rating_update',
      'wrapper' => 'items_rating',
      'effect' => 'fade',
      'speed' => 'slow',
    );

    // Add the weight of the component.
    $form['extra']['component_weight'] = array(
      '#type' => 'textfield',
      '#size' => 3,
      '#title' => t('Weight'),
      '#description' => t('Please input the component weight, which is a number between 0 and 100 representing the percentage of how important is this component within the entire set of components of this webform.'),
      '#default_value' => isset($component['extra']['component_weight']) ? $component['extra']['component_weight'] : '',
      '#field_suffix' => '%',
    );

    $form['#validate'][] = 'webform_rating_form_webform_component_edit_form_validate';
  }

  /**
   * Helper function for the edit component form validate.
   */
  public static function webformCcomponentEditFormValidate(&$form, &$form_state) {
    if (!empty($form_state['values']['extra']['items_rating'])) {
      foreach ($form_state['values']['extra']['items_rating'] as $key => $value) {
        // @todo; should we allow negative numbers here?
        if (!is_numeric($value) || $value < 0) {
          form_error($form['extra']['items_rating'][$key], t('The number of points must be numeric and not smaller than 0!'));
        }
      }
    }
    // Validate the weight, if any.
    $component_weight = trim($form_state['values']['extra']['component_weight']);
    form_set_value($form['extra']['component_weight'], $component_weight, $form_state);
    if ($component_weight !== '' && (!is_numeric($component_weight) || $component_weight < 0 || $component_weight > 100)) {
      form_error($form['extra']['component_weight'], t('The weight value is incorrect, it must be a number between 0 and 100.'));
    }
  }
}
