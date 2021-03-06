<?php

/**
 * @file
 * Definition of views_handler_field_taxonomy.
 */

/**
 * Field handler to provide simple renderer that allows linking to a taxonomy
 * term.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_taxonomy extends views_handler_field {
  /**
   * Constructor to provide additional field to add.
   *
   * This constructer assumes the taxonomy_term_data table. If using another
   * table, we'll need to be more specific.
   */
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = 'vid';
    $this->additional_fields['tid'] = 'tid';
    $this->additional_fields['name'] = 'name';
    $this->additional_fields['vocabulary_machine_name'] = array(
      'table' => 'taxonomy_vocabulary',
      'field' => 'machine_name',
    );
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['case'] = array('default' => 'none');
    $options['link_to_taxonomy'] = array('default' => FALSE, 'bool' => TRUE);
    $options['convert_spaces'] = array('default' => FALSE, 'bool' => TRUE);
    return $options;
  }

  /**
   * Provide link to taxonomy option
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_taxonomy'] = array(
      '#title' => t('Link this field to its taxonomy term page'),
      '#description' => t("Enable to override this field's links."),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_taxonomy']),
    );
     $form['convert_spaces'] = array(
      '#title' => t('Convert spaces in term names to hyphens'),
      '#description' => t('This allows links to work with Views taxonomy term arguments.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['convert_spaces']),
    );
    $form['case'] = array(
      '#type' => 'select',
      '#title' => t('Case'),
      '#description' => t('When taxonomy term field, select how to transform the case of the filter value.'),
      '#options' => array(
        'none' => t('No transform'),
        'upper' => t('Upper case'),
        'lower' => t('Lower case'),
        'ucfirst' => t('Capitalize first letter'),
        'ucwords' => t('Capitalize each word'),
      ),
      '#default_value' => !empty($this->options['case']) ? $this->options['case'] : 'none',
      '#fieldset' => 'more',
    );
    parent::options_form($form, $form_state);
  }

  /**
   * Render whatever the data is as a link to the taxonomy.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    $tid = $this->get_value($values, 'tid');
    if (!empty($this->options['link_to_taxonomy']) && !empty($tid) && $data !== NULL && $data !== '') {
      $term = new stdClass();
      $term->tid = $tid;
      $term->vid = $this->get_value($values, 'vid');
      $term->name = $this->get_value($values, 'name');
      $term->vocabulary_machine_name = $values->{$this->aliases['vocabulary_machine_name']};
      $this->options['alter']['make_link'] = TRUE;
      $uri = entity_uri('taxonomy_term', $term);
      if (isset($uri['options'])) {
        $this->options['alter'] = array_merge($this->options['alter'], $uri['options']);
      }
      $this->options['alter']['path'] = $uri['path'];
      // If entity_uri() returned an options array, use it.
      if (isset($uri['options'])) {
        $this->options['alter'] = $uri['options'] + $this->options['alter'];
      }
    }

    if (!empty($this->options['convert_spaces'])) {
      $data = str_replace(' ', '-', $data);
    }

    if (!empty($this->options['case']) && $this->options['case'] !== 'none') {
      $data = $this->case_transform($data, $this->options['case']);
    }

    return $data;
  }

  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
}
