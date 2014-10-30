<?php
/**
 * @file
 * Implementation of views_row_plugin
 */

class viewsPluginRowViewsMosaic extends views_plugin_row_fields {

  function option_definition() {
    $options = parent::option_definition();
    $options['backdrop'] = array('default' => NULL);
    $options['mosaic_options']['opacity'] = array('default' => 1);
    return $options;
  }

  /**
   * Provide a form for setting options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $options = $this->display->handler->get_field_labels();

    $form['backdrop'] = array(
      '#title' => t('Backdrop'),
      '#type' => 'select',
      '#default_value' => isset($this->options['backdrop']) ? $this->options['backdrop'] : -1,
      '#description' => t('Choose the field to be backdrop image. All other fields will be overlay'),
      '#options' => $options,
    );

    $theme_options = array(
      'custom' => t('Custom'),
      'circle' => t('Circle'),
      'fade' => t('Fade'),
      'bar' => t('Bar'),
      'bar2' => t('Bar with content'),
      'bar3' => t('Top bar'),
      'cover' => t('Cover right'),
      'cover2' => t('Cover top'),
      'cover3' => t('Cover top right')
    );

    $form['theme'] = array(
      '#type' => 'select',
      '#title' => t('Theme'),
      '#description' => t('Theme from mosaic jquery examples'),
      '#options' => $theme_options,
      '#default_value' => isset($this->options['theme']) ? $this->options['theme'] : "fade",
    );

    $form['mosaic_options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Mosaic Options'),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          '#edit-row-options-theme' => array('value' => 'custom'),
        ),
      ),
    );

    //List opacity options from 0 to 1 (0.1, 0.2, ..)
    $opacity_options = range(0, 1, .1);
    unset($opacity_options[0]);
    $form['mosaic_options']['opacity'] = array(
      '#type' => 'select',
      '#title' => t('Opacity'),
      '#description' => t('The final opacity of overlay'),
      '#options' => $opacity_options,
      '#default_value' => isset($this->options['mosaic_options']['opacity']) ? $this->options['mosaic_options']['opacity'] : 1,
    );

    $form['mosaic_options']['speed'] = array(
      '#type' => 'textfield',
      '#title' => t('Speed'),
      '#description' => t('Speed of animation in milesconds'),
      '#default_value' => isset($this->options['mosaic_options']['speed']) ? $this->options['mosaic_options']['speed'] : 150,
    );

    $animation_options = array(
      'fade' => 'Fade',
      'slide' => 'Slide'
    );

    $form['mosaic_options']['animation'] = array(
      '#type' => 'select',
      '#title' => t('Animation type'),
      '#description' => t('Fade or slide mosais'),
      '#options' => $animation_options,
      '#default_value' => isset($this->options['mosaic_options']['animation']) ? $this->options['mosaic_options']['animation'] : "fade",
    );
  }
}
