<?php
/**
 * @file mtheme.admin.inc
 *  admin settings page
 */
/**
  * Form builder. Configure Module Theme settings.
  *
  * @ingroup forms
  * @see system_settings_form().
  */
function mtheme_admin_settings() {
  $form['mtheme_compile'] = array(
    '#type' => 'checkbox',
    '#title' => t('Compile Module Theme CSS into a single stylesheet'),
    '#default_value' => variable_get('mtheme_compile', FALSE),
  );

  return system_settings_form($form);
}
