<?php
/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'mtheme_selector', // As defined in hook_schema(), mymodule_myobj.
  'access' => 'administer mtheme', // Define a permission users must have to access these pages.
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
    //'validate' => 'mtheme_ctools_export_ui_edit_item_form_validate', 
    // 'submit' and 'validate' are also valid callbacks.
  ),
  // Use this to modify the default export UI interface.
  /*
  'handler' => array(
    'class' => 'mtheme_export_ui',
    'parent' => 'ctools_export_ui',
  ),
  // */
);

/**
 * Define the preset add/edit form.
 */
function mtheme_ctools_export_ui_form(&$form, &$form_state) {
  if (arg(5) == 'edit') {
    $selector = $form_state['item'];
  }
  else if (arg(5) == 'clone') {
    $name = arg(4);
    $selector = mtheme_selector_load($name);
  } else {
    $selector = FALSE;   
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
  $title = ($selector) ? $selector->title : '';
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Human readable CSS selector name. '
                       . 'This will be included in .css file as a comment '
                       . 'above the selector.'),
    '#default_value' => $title,
    '#required' => FALSE,
  );
  $css_selector = ($selector) ? $selector->css_selector : '';
  $form['css_selector'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS Selector'),
    '#description' => t('CSS selector. E.g. div or  div#example .demo '),
    '#default_value' => $css_selector,
    '#required' => true,
    '#rows' => 1,
  );
  $css_properties = ($selector) ? $selector->css_properties : '';
  $form['css_properties'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS Properties'),
    '#default_value' => $css_properties,
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
    $form['files']['file_upload'] = array(
      '#type' => 'file',
      '#title' => t('Upload image file'),
      '#size' => 30,
      '#description' => t('Upload files to use in your CSS.'),
    );
    // Check for available files
    $path = mtheme_images_path($selector); //Path to files dir or features module w files.
    $i = 0; // Use counter in case of conflicting $file->name values
    foreach (@file_scan_directory($path, '/.*/') as $file) {
      $title = "images/$file->filename";
      $link = l($title, $file->uri);
      // Add form items
      $item = $file->name . '-' . $i;
      $i++;
      $form['files'][$item] = array(
        '#type' => 'item',
        '#title' => $link,
        '#description' => t("Add this file to your CSS like this: "
                           . "background: url('!title');", 
                           array('!title' => $title)),
      );
    }
  }
  else if (arg(3) == 'add') {
    $form['files'] = array(
      '#type' => 'item',
      '#title' => t('Files'),
      '#description' => t('Save then click Edit to add background files here.'),
    );
  }
  $grp = ($selector) ? $selector->grp : ''; 
  $form['grp'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS Selector Group'),
    '#description' => t("Group selectors together. (Use your module name "
                       . "or site name if you're not sure what to put here.)"),
    '#default_value' => $grp,
    '#required' => TRUE,
  );
  $description = ($selector) ? $selector->description : '';
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Optional description.'),
    '#default_value' => $description,
    '#required' => FALSE,
  );
  $weight = ($selector) ? $selector->weight : '';
  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('Sort order of selectors for multiple identical selectors.'),
    '#default_value' => $weight,
    '#required' => FALSE,
  );
  // Keep track of whether the subtheme was in code only when form
  // was built.
  $in_code_only = ($selector) ? $selector->in_code_only : FALSE;
  $form['in_code_only'] = array(
    '#type' => 'hidden',
    '#value' => $in_code_only,
  );
}

/**
 * After form passes Ctools validation, validate it here.
 * Prepare file to be saved.
 */
function mtheme_ctools_export_ui_edit_item_form_validate(&$form, &$form_state) {
  // Handle file uploads.
  // This is the validator used for uploading a theme logo.
  $validators = array('file_validate_is_image' => array());
  // These validators are used for uploading favicons. 
  // @todo Would these validators be more appropriate here?
  //$validators = array('file_validate_extensions' => array('ico png gif jpg jpeg apng svg'));

  // Check for a new uploaded image.
  $file = file_save_upload('file_upload', $validators);
  if (isset($file)) {
    // File upload was attempted.
    if ($file) {
      // Put the temporary file in form_values so we can save it on submit.
      $form_state['values']['file_upload'] = $file;
    }
    else {
      // File upload failed.
      form_set_error('file_upload', t('The file could not be uploaded.'));
      $form_state['values']['file_upload'] = FALSE; // TODO does this solve the undefined index error on submit? 
    }
  }
}

/**
 * When a selector is saved (added, edited or cloned)
 * re-write css for that selector.
 *
 * When a selector changes states from in_code_only to
 * stored in the database + files directory, copy image files
 * from features module into files directory.
 */
function mtheme_ctools_export_ui_edit_item_form_submit(&$form, &$form_state) {
  ctools_include('css', 'mtheme'); // include includes/css.inc
  $selector = $form_state['item'];
  $values = $form_state['values'];
  $in_code_only = $form_state['values']['in_code_only'];
  $clone = (arg(5) == 'clone') ? TRUE : FALSE; // Existing selector is being cloned.

  // Handle files.

  // If the user uploaded a new file, save it to a permanent location.
  if ($file = $values['file_upload']) {
    unset($values['file_upload']);
    $source = $file->uri;
    $destination = array();
    // Save file to selector-specific directory here:
    // files/$selector_name/images/imageX.jpg
    $destinations[1] = mtheme_images_path($selector);
    // Make another copy available for compile.css here:
    // files/mtheme/compile/images.
    $destinations[2] = mtheme_images_path($selector, 'compile');
    foreach ($destinations as $destination) {
      if (!file_prepare_directory($destination, FILE_CREATE_DIRECTORY)) {
        $txt = t("!destination could not be created.", 
               array('!destination' => $destination));
        drupal_set_message($txt);
      }
      if ($filename = file_unmanaged_copy($source, $destination, FILE_EXISTS_REPLACE)) {
        $txt = t("New file saved: ") . l($filename, $filename);
        drupal_set_message($txt);
      }
    }
  }

  // If the selector is being overridden (stored in a module and now modified 
  // through the GUI) or cloned, copy image files to public files directory.
  if ($in_code_only || $clone) {
    $paths = array();
    if ($in_code_only) { // Was in-code-only before submit. Not any more. Copy files.
// TODO test this
      $paths['from'] = mtheme_images_path($selector, 'in code only');
      $paths['to'] = mtheme_images_path($selector, 'files');
    }
    else if ($clone) {
// TODO test this
      $name = arg(4);
      $original_selector = mtheme_selector_load($name);
      $paths['from'] = mtheme_images_path($original_selector);
      $paths['to'] = mtheme_images_path($selector, 'files');
    }
    // First copy files to the selector's images directory here:
    // files/mtheme/$selector/images.
    mtheme_copy_files($selector, $paths);
    // Now copy files to compiled images directory here:
    // files/mtheme/compile/images.
// TODO test this
    $paths['to'] = mtheme_images_path(NULL, 'compile');
    mtheme_copy_files($selector, $paths);
  }

  // Now handle CSS.

  // (Re-)write selector.css.
  mtheme_css($selector); // @todo add error handling here?

  // Rebuild mtheme cache by recompiling and rewriting compile.css.
  $compile = TRUE;
  mtheme_css($selector, $compile);
 
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
  $public = new DrupalPublicStreamWrapper;
  $files_directory = $public->getDirectoryPath();
  $path = $files_directory . "/mtheme/$name/$name.css";
  file_unmanaged_delete($path);
  // delete image files
  $path = mtheme_images_path($selector);
  //foreach (@file_scan_directory($path, '.*') as $file) {
  foreach (@file_scan_directory($path, '/.*/') as $file) {
    file_unmanaged_delete($file->filename);
  }
  // remove directories
  $path = $files_directory . "/mtheme/$name/images";
  if (file_prepare_directory($path)) {
    $result = drupal_rmdir($path);
    if ($result) {
      $txt = $path . t(' deleted.');
      drupal_set_message($txt);
    }
    else {
      $txt = $path . t(' could not be deleted.');
    }
  }
  $path = $files_directory . "/mtheme/$name";
  if (file_prepare_directory($path)) {
    $result = drupal_rmdir($path);
    if ($result) {
      $txt = $path . t(' deleted.');
      drupal_set_message($txt);
    }
    else {
      $txt = $path . t(' could not be deleted.');
    }
  }
}
