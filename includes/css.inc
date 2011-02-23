<?php

/**
 * @file mtheme/includes/css.inc
 *  This file contains all the functions that write
 *  CSS for the mtheme module.
 */

/**
 * (Re-)Write style sheet(s).
 *
 * Stylesheets are organized like this:
 *   files/mtheme/selector_name1.css
 *   files/mtheme/selector_name2.css
 *   files/mtheme/selector_name3.css
 *   files/mtheme/selector_nameN.css
 *
 * @param $selector
 *  object, new mtheme_selector
 *
 * @param $compile
 *  boolean,
 *  - TRUE: compile all mtheme selectors
 *  - FALSE: only write individual selectors
 */
function mtheme_css($selector = NULL, $compile = FALSE) {
  $css = mtheme_generate_css($selector, $compile);
  $written = mtheme_write_css($css, $compile);
  if (!$written) {
    // @todo handle error here. Notify user.
  }
  return $css;
}


/**
 * Callback for admin/build/mtheme/css, (re-)build mtheme CSS cache.
 *
 * Compile CSS from all selectors, rewrite mtheme.css
 * and re-copy all relevant files to mtheme/mtheme/images.
 */
function mtheme_rebuild_css() {
  $link = l(t('Configure Module Theme.'), 'admin/config/mtheme');
  $txt = t('All your Module Theme selectors have been (re-)compiled '
    . 'into the stylesheet below. Improve performance by configuring Module Theme '
    . 'to always load this compiled stylesheet. '
    . 'Note: When you enable/disable modules with new module theme elements, '
    . 'this stylesheet will have to be rebuilt for changes to take effect. '
    . '!settings ',
    array('!settings' => $link));
  drupal_set_message($txt);

  // (Re)-write CSS.
  $css = mtheme_css(NULL, TRUE);
  // Refresh files, copy originals for each selector mtheme/mtheme/images.
  $selectors = ctools_export_load_object('mtheme_selector');
  $paths = array();
  foreach ($selectors as $selector) {
    $paths['from'] = mtheme_images_path($selector);
    $paths['to'] = mtheme_images_path($selector, 'mtheme');
    mtheme_copy_files($selector, $paths);
  }
  // Return CSS as output to display for user to see.
  $output = theme('mtheme_css_page', $css);

  return $output;
}

/**
 * Generate CSS for a single selector
 * or for all mtheme selectors.
 *
 * mtheme.css:
 *   (This is currently only used for
 *   documentation.)
 *   Get all the mtheme CSS selectors,
 *   organize them into groups,
 *   sort by weight (within groups),
 *   print CSS.
 *
 * @param $selector
 *  object, new mtheme_selector
 *
 * @param $compile
 *  boolean, $compile all selectors into a single stylesheet: mtheme.css.
 *  mtheme.css is only used for documentation.
 */
function mtheme_generate_css($new_selector = NULL, $compile = FALSE) {
  $css = _mtheme_start_css();

  // TODO Check what happens here with new selectors. If they're not included in
  //      $groups, make a way to include them.
  if ($new_selector && !$compile) {
    $name = $new_selector->name;
    $css = _mtheme_write_selector_css($css, $new_selector);
  }
  else if ($compile) {
    // Include $new_selector in $groups, if a new selector is being saved.
    if ($new_selector) {
      $ns = array();
      $ns[$new_selector->name] = $new_selector;
    }
    else {
      $ns = NULL;
    }
    if ($groups = mtheme_get_groups(NULL, $ns)) {
      $name = 'mtheme';
      foreach ($groups as $group) {
        $css = _mtheme_group_comment($css, $group);
        foreach ($group['selectors'] as $selector) {
          if ($selector->grp == $group['group_title'] && $selector->name != $new_selector->name) {
            $css = _mtheme_write_selector_css($css, $selector);
          }
          else { // A selector is being updated. Use the new one.
            $css = _mtheme_write_selector_css($css, $new_selector);
          }
        }
      }
    }
    else {
      $name = 'mtheme';
      $txt = t('there are no mtheme modules enabled');
      $css .= "\n  /* $txt */ ";
    }
  }

  return array('name' => $name, 'css' => $css);
}


/**
 * Compose mtheme.css based on settings entered by user.
 */
function mtheme_compose_css($a) {
  // write CSS
  $css = '';
  foreach ($a as $selector => $array) {
    $css .= $selector . "\n{\n";
    foreach ($array as $property => $value) {
      $css .= ($value) ?  "  $property: $value;\n" : '';
    }
    $css .= "}\n\n";
  }

  return $css;
}

/**
 * Write CSS and store it
 * in files/mtheme/name/name.css.
 *
 * @param $css
 *  Array. $css = array('name' => $name, 'css' => $css);
 *
 * @param $compile
 *  boolean, TRUE -> compile all selectors and write a single stylesheet.
 *
 * @param
 */
function mtheme_write_css($css, $compile = FALSE) {
  $file_contents = $css['css'];
  // Build destination URI for CSS file.
  $mtheme_path = file_build_uri('mtheme');
  if ($compile) {
    $name = 'compile';
  }
  else {
    $name = $css['name'];
  }
  $destination = $mtheme_path . "/$name/$name.css";
  // Normalize $destination for good measure. (This step is probably unnecessary.)
  $destination = file_stream_wrapper_uri_normalize($destination);

  // If the directory doesn't exist, create it
  if (!file_prepare_directory(drupal_dirname($destination), FILE_CREATE_DIRECTORY)) {
    $txt = t('The function file_prepare_directory() failed. Please notify your system administrator.');
    drupal_set_message($txt, 'error');
  }

  // Create CSS file.
  if (!file_save_data($file_contents, $destination, FILE_EXISTS_REPLACE)) {
    $txt = t('The function file_save_data() failed. Please notify your system administrator.');
    drupal_set_message($txt, 'error');
  }

  // Notify user about new CSS
  $txt = t('New CSS file created: !file',
          array('!file' => l("$name.css", file_create_url($destination))));

  drupal_set_message($txt);
}
