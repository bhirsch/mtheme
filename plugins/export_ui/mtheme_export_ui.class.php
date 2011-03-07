<?php
/**
 * CTools export UI extending class. Slightly customized for Context.
 *
 * Based on: context/context_ui/export_ui/context_export_ui.class.php.
 * 
 * @todo Figure out why certain group names break the table layout, e.g. try ...
 *  test, test 1 and test2
 */
class mtheme_export_ui extends ctools_export_ui {
  /*
  function list_form(&$form, &$form_state) {
    parent::list_form($form, $form_state);
    $form['top row']['submit'] = $form['bottom row']['submit'];
    $form['top row']['reset'] = $form['bottom row']['reset'];
    $form['bottom row']['#access'] = FALSE;
    // Invalidate the context cache.
    context_invalidate_cache();
    return;
  }
  // */

  function list_css() {
    ctools_add_css('export-ui-list');
    drupal_add_css(drupal_get_path("module", "mtheme") ."/mtheme.css");
  }

  function list_render(&$form_state) {
    /*
    $list_header = $this->list_table_header();
    $title = array(array('data' => 'Title', 'class' => 'ctools-export-ui-name'));
    $list_header = array_merge($title, $list_header);
    // */
    
    $list_table_header = $this->list_table_header();
    $list_table_header[0]['data'] = t('CSS Selector');

    return theme('table', $list_table_header, $this->rows, array('class' => 'mtheme-admin', 'id' => 'ctools-export-ui-list-items'));
  }

  function list_build_row($item, &$form_state, $operations) {
    $name = $item->name;

    // Add a row for groups (feature sets, "tags" in context module).
    $group = !empty($item->grp) ? $item->grp : t('< No Group >');
    if (!isset($this->rows[$group])) {
      $this->rows[$group]['data'] = array();
      $this->rows[$group]['data'][] = array('data' => check_plain($group), 'colspan' => 3, 'class' => 'group');
      $this->sorts["{$group}"] = $group;
    }

    // Build row for each item.
    $this->rows["{$group}:{$name}"]['data'] = array();
    $this->rows["{$group}:{$name}"]['class'] = !empty($item->disabled) ? 'ctools-export-ui-disabled' : 'ctools-export-ui-enabled';

    $title = ($item->title) ? check_plain($item->title) .', '. check_plain($name) : check_plain($name);
    $output = '<div subtheme-export-ui-title>/*** '. $title .' ***/</div>';
    $output .= '<div subtheme-export-ui-css-selector>'. check_plain($item->css_selector) .'</div>';
    //$output = check_plain($name) .' ('. check_plain($item->css_selector) .')';
    $output .= "<div class='description'>" . check_plain($item->description) . "</div>";

    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => $output,
      'class' => 'ctools-export-ui-name'
    );
    /*
    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => check_plain($item->css_selector),
      'class' => 'ctools-export-ui-css-selector'
    );
    // */
    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => check_plain($item->type),
      'class' => 'ctools-export-ui-storage'
    );
    $this->rows["{$group}:{$name}"]['data'][] = array(
      'data' => theme('links', $operations, array('class' => 'links inline')),
      'class' => 'ctools-export-ui-operations'
    );

    // Sort by group, name.
    $this->sorts["{$group}:{$name}"] = $group . $name;
  }
  
  /**
   * Override of edit_form_submit().
   * Don't copy values from $form_state['values'].
   */
  /*
  function edit_form_submit(&$form, &$form_state) {
    if (!empty($this->plugin['form']['submit'])) {
      $this->plugin['form']['submit']($form, $form_state);
    }
  }
  // */
}


