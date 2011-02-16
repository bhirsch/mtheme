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
  if (arg(5) == 'edit') {
    $selector = $form_state['item'];
  } else if (arg(5) == 'clone') {
    $name = arg(4);
    $selector = mtheme_selector_load($name);
  }

  // mtheme's configuration interface.
  /* Ctools includes this field automatically on its own.
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
    '#type' => 'textarea',
    '#title' => t('CSS Selector'),
    '#description' => t('CSS selector. E.g. div or  div#example .demo '),
    '#default_value' => $selector->css_selector,
    '#required' => true,
    '#rows' => 1,
  );
  $form['css_properties'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS Properties'),
    '#default_value' => $selector->css_properties,
    '#description' => t('E.g. background-color: white; <br/> font-size: 12px; '),
    '#required' => true,
  );
  // Include files field group for uploading background images
  // Include enctype, otherwise uploads won't work!
  if (arg(5) == 'edit' || arg(5) == 'clone') { // Only add files after first save, we need a name.
    $form['#attributes'] = array('enctype' => 'multipart/form-data');
    $form['files'] = array(
      '#type' => 'fieldset',
      '#title' => t('Files'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['files']['file'] = array(
      '#type' => 'file',
      '#title' => t('Files'),
      '#size' => 30,
      '#element_validate' => array('mtheme_save_files'),
    );
    // Check for available files
    $path = mtheme_images_path($selector); //Path to files dir or features module w files.
    $i = 0; // Use counter in case of conflicting $file->basename values
    foreach (@file_scan_directory($path, '.*') as $file) {
      $title = "images/$file->basename";
      $link = l($title, $file->filename);
      // Add form items
      $item = $file->basename .'-'. $i;
      $form['files'][$item] = array(
        '#type' => 'item',
        '#title' => t("File "). $i,
        '#value' => $link,
        '#description' => t("Add this file to your CSS like this: "
                           ."background: url('". $title ."');"),
      );
      $i++; 
    }
  } else if (arg(3) == 'add') {  
    $form['files'] = array(
      '#type' => 'item',
      '#title' => t('Files'),
      '#value' => t('Save then click Edit to add background files here.'),
    );  
  }
  $form['grp'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Selector Group'),
    '#description' => t("Group selectors together. (Use your module name "
                       ."or site name if you're not sure what to put here.)"),
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
  // Keep track of whether the subtheme was in code only when form 
  // was built.
  $form['in_code_only'] = array(
    '#type' => 'hidden',
    '#value' => $selector->in_code_only,
  );
}

/**
 * When a selector is saved (added, edited or cloned)
 * re-write css for that selector. 
 * 
 * When a selector changes states from in_code_only to 
 * stored in the database + files directory, copy image files
 * from features module into files directory.
 * 
 * @todo There's probably a more efficient way to do this than
 * re-writing the CSS after each new selector is saved, but 
 * for now, doing it this way keeps it simple. 
 * 
 * There is no extravagant naming convention magic here.
 * This function is just called mymodule_ctoolform_submit, that is
 * mtheme _ ctools_export_ui_edit_item_form _ submit.
 */
function mtheme_ctools_export_ui_edit_item_form_submit(&$form, &$form_state) {
  ctools_include('css', 'mtheme'); // include includes/css.inc

  $selector = $form_state['item'];
  $in_code_only = $form_state['values']['in_code_only'];
  $clone = (arg(5) == 'clone') ? TRUE : FALSE; // Existing selector is being cloned.

  // (Re-)write css.
  mtheme_css($selector); // @todo add error handling here?
  // Copy files.
  if ($in_code_only || $clone) {
    $paths = array();
    if ($in_code_only) { // Was in-code-only before submit. Not any more. Copy files.
      $paths['from'] = mtheme_images_path($selector, 'in code only');
      $paths['to'] = mtheme_images_path($selector, 'files');
    } else if ($clone) {
      $name = arg(4);
      $original_selector = mtheme_selector_load($name);
      $paths['from'] = mtheme_images_path($original_selector);
      $paths['to'] = mtheme_images_path($selector, 'files');
    }
    mtheme_copy_files($selector, $paths);
  }
}

/**
 * When a selector is deleted or reverted,
 * delete the files/mtheme/myselector.css
 * and any related files.
 */
function mtheme_ctools_export_ui_delete_confirm_form_submit(&$form, &$form_state) {
  $name = $form_state['item']->name;
  $selector = mtheme_selector_load($name);
  // delete style sheet
  $path = file_directory_path() ."/mtheme/$name/$name.css";
  file_delete($path);
  // delete image files 
  $path = mtheme_images_path($selector);  
  foreach (@file_scan_directory($path, '.*') as $file) {
    file_delete($file->filename);
  }
  // remove directories
  $path = file_directory_path() ."/mtheme/$name/images";
  if (file_check_directory($path)) {
    $result = rmdir($path);
    if ($result) {
      $txt = $path . t(' deleted.');  
      drupal_set_message($txt);
    } else {
      $txt = $path . t(' could not be deleted.'); 
    }
  }
  $path = file_directory_path() ."/mtheme/$name";
  if (file_check_directory($path)) {
    $result = rmdir($path);
    if ($result) {
      $txt = $path . t(' deleted.');  
      drupal_set_message($txt);
    } else {
      $txt = $path . t(' could not be deleted.'); 
    }
  }
}
