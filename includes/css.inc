<?php 
/**
 * @file mtheme/includes/css.inc
 *  This file contains all the functions that write
 *  custom CSS for the mtheme module.
 */

/**
 * Get all the mtheme CSS selectors,
 * organize them into groups,
 * sort by weight (withing groups), // @todo add weights to groups?
 * print CSS.  
 */
function mtheme_generate_css() {
  $groups = mtheme_get_groups();
//dsm($groups, '$groups');
  $css = _mtheme_start_css();
  foreach ($groups as $group) {
//dsm($group, 'foreach ($groups as $group)');
    $css = _mtheme_group_comment($css, $group); 
    foreach ($group['selectors'] as $selector) {
//dsm($selector, 'foreach($selectors as $selector)');
      if ($selector->grp == $group['group_title']) {
        $css = _mtheme_write_selector_css($css, $selector); 
      }   
    }
  }

  return $css; 
}

/**
 * Write CSS comment for group of selectors.
 */
function _mtheme_group_comment($css = NULL, $group) {
  $txt = t($group['group_title']) .' ';
  $line = 60 - strlen($txt);
  for ($i = 0; $i < $line; $i++) {
    $txt .= '=';
  }

  $css .= "\n";
  $css .= '/**'. "\n";
  $css .= ' * '. $txt ."\n";
  $css .= ' */'. "\n";
  $css .= "\n";

  return $css; 
}

/**
 * Write CSS for selector.
 */
function _mtheme_write_selector_css($css = NULL, $selector) {
  // Add comment.
  $css = _mtheme_selector_comment($css, $selector); 
  // Add selector and properties. 
  $css .= '  '. $selector->css_selector ."\n";
  $css .= "  {\n";
  $properties = explode("\n", $selector->css_properties);
  foreach($properties as $property) {
    $css .= "    $property"; // seems like $property hangs onto carriage return 
  }
  $css .= "\n  }\n";

  return $css; 
}

/**
 * Write CSS comment for selector.
 */
function _mtheme_selector_comment($css, $selector) {
  $txt = '';
  $txt = t($selector->title) .' -- ';
  $txt .= t('unique identifier:') . $selector->name; 

  $css .= "  /* $txt */ \n";

  return $css; 
}

/**
 * Begin CSS with timestamp in comment at the top.
 */
function _mtheme_start_css() {
  $time = time();
  $date = date("F j, Y, g:i a");
  global $base_url;

  $txt = "Generated $date -- $time -- $base_url";  

  $css = '';
  $css .= "/* $txt */ \n \n";  
  
  return $css; 
}

/**
 * When a selector is saved (added or edited)
 * re-write css to include new selector. 
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
  //$selector = $form_state['item'];
  //dsm($selector);
  $css = mtheme_generate_css();   
  $written = mtheme_write_css($css);
  if (!$written) {
    // TODO handle error here. Notify user.  
  }
}

/**
 * Compose custom.css based on settings entered by user.
 */
function mtheme_compose_css($a) {
    // write CSS
    $css = ''; 
    foreach ($a as $selector => $array) {
      $css .= $selector ."\n{\n";
      foreach ($array as $property => $value) {
        $css .= ($value) ?  "  $property: $value;\n" : '' ;
      }
      $css .= "}\n\n";
    }

  return $css; 
}

/**
 * Write custom.css and store it 
 * in files/mtheme/custom.css.
 */ 
function mtheme_write_css($file_contents) {
  // Set the location of the custom.css file
  $file_path = file_directory_path() .'/mtheme/custom.css';

  // If the directory doesn't exist, create it
  $dir = file_check_directory(dirname($file_path), FILE_CREATE_DIRECTORY);
  if (!$dir) {
    $txt = t('The function file_check_directory() failed. Please notify your system administrator.');
    drupal_set_message($txt, 'error');
  }

  // Create CSS file.
  $saved = file_save_data($file_contents, $file_path, FILE_EXISTS_REPLACE);
  if (!$saved) {
    $txt = t('The function file_save_data() failed. Please notify your system administrator.'); 
    drupal_set_message($txt, 'error');
  }

  // Notify user about new CSS 
  $txt = t('New custom CSS file created: !file',
          array('!file' => l($file_path, $file_path)));

  drupal_set_message($txt);
}
