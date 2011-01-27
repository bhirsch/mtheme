<?php
/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'mtheme_selector',  // As defined in hook_schema(), mymodule_myobj.
  'access' => 'administer mtheme',  // Define a permission users must have to access these pages.

  // Define the menu item.
  'menu' => array(
    'menu item' => 'mtheme',
    'menu title' => t('Module Theme'),
    'menu description' => t('Administer Module Theme selectors.'),
  ),

  // Define user interface texts.
  'title singular' => t('selector'),
  'title plural' => t('selectors'),
  'title singular proper' => t('Module Theme Selector'),
  'title plural proper' => t('Module Theme Selectors'),
  // Define the names of the functions that provide the add/edit forms.
  'form' => array(
    'settings' => 'mtheme_ctools_export_ui_form',
    // 'submit' and 'validate' are also valid callbacks.
  ),
  // Use this to modify the default export UI interface.
  'handler' => array(
    'class' => 'mtheme_export_ui',
    'parent' => 'ctools_export_ui',
  ),
);

/**
* Define the preset add/edit form.
*/
function mtheme_ctools_export_ui_form(&$form, &$form_state) {
  $selector = $form_state['item'];

  // mtheme's configuration interface.
  /* Apparently Ctools includes this field automatically on its own.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Machine Name'),
    '#description' => t('lowercase, no spaces or punctuation'),
    '#default_value' => $selector->name,
    '#required' => true,
  );
  // */
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Human readable CSS selector name. '
                       .'This will be included in .css file as a comment '
                       .'above the selector.'),
    '#default_value' => $selector->title,
    '#required' => FALSE,
  );
  $form['css_selector'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Selector'),
    '#description' => t('CSS selector. E.g. div or  div#example .demo '),
    '#default_value' => $selector->css_selector,
    '#required' => true,
  );
  $form['css_properties'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS Properties'),
    '#default_value' => $selector->css_properties,
    '#description' => t('E.g. background-color: white; <br/> font-size: 12px; '),
    '#required' => true,
  );
  $form['grp'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Selector Group'),
    '#description' => t("Group selectors together. (Use your module name if you're not sure what to put here.)"),
    '#default_value' => $selector->grp,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Optional description.'),
    '#default_value' => $selector->description,
    '#required' => FALSE,
  );
  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('Sort order of selectors for multiple identical selectors.'),
    '#default_value' => $selector->weight,
    '#required' => FALSE,
  );

}
