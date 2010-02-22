<?php
// $Id$

/**
 * @file
 * Quickedit node form
 *
 * @author Stijn De Meyere
 */
 
 function quickedit_node() {
   $nid = arg(1);
   $node = node_load($nid);
   if (node_access('update', $node)) {
     return drupal_get_form('quickedit_nodeoptions_form', $node);
   }
 }
 
 function quickedit_nodeoptions_form($form_state, $node) {
   $form = array();
   
   $form['panes'] = array(
     '#tree' => FALSE,
     '#type' => 'admin_panes',
   );
   
   $form['panes']['general-options'] = array(
     '#title' => t('General options'),
   );
   
   $form['panes']['general-options']['nid'] = array(
     '#type' => 'value',
     '#value' => $node->nid
   );
   
   $form['panes']['general-options']['node'] = array(
     '#type' => 'value',
     '#value' => $node
   );
   
   $form['panes']['general-options']['title'] = array(
     '#type' => 'textfield',
     '#title' => t('Title'),
     '#default_value' => $node->title,
     '#required' => TRUE
   );
   
   $form['panes']['general-options']['author'] = array(
     '#type' => 'textfield',
     '#title' => t('Author'),
     '#maxlength' => 60,
     '#autocomplete_path' => 'user/autocomplete',
     '#default_value' => $node->name,
     '#description' => t('Leave blank for %anonymous.', array('%anonymous' => variable_get('anonymous', t('Anonymous')))),
     '#required' => TRUE
   );
   
   $form['panes']['general-options']['created'] = array(
     '#type' => 'textfield',
     '#title' => t('Published'),
     '#default_value' => format_date($node->created, 'custom', 'Y-m-d H:i:s O'),
     '#description' => t('Format: %time. Leave blank to use the time of form submission.', array('%time' => !empty($node->date) ? $node->date : format_date($node->created, 'custom', 'Y-m-d H:i:s O'))),
     '#required' => TRUE
   );
   
   // Node options for administrators
   
   $form['panes']['general-options']['options']['status'] = array(
     '#type' => 'checkbox',
     '#title' => t('Published'),
     '#default_value' => $node->status,
   );
   $form['panes']['general-options']['options']['promote'] = array(
     '#type' => 'checkbox',
     '#title' => t('Promoted to front page'),
     '#default_value' => $node->promote,
   );
   $form['panes']['general-options']['options']['sticky'] = array(
     '#type' => 'checkbox',
     '#title' => t('Sticky at top of lists'),
     '#default_value' => $node->sticky,
   );
   
   if (module_exists('comment')) {
     $form['panes']['comment-settings'] = array(
       '#title' => t('Comment settings'),
     );
     
     $form['panes']['comment-settings']['comment'] = array(
       '#type' => 'radios',
       '#parents' => array('comment'),
       '#default_value' => $node->comment,
       '#options' => array(t('Disabled'), t('Read only'), t('Read/Write')),
       '#access' => user_access('administer comments'),
     );
   }  
   
   //TODO check if scheduler is enable for this content type
   if (module_exists('scheduler') && user_access('schedule (un)publishing of nodes') && variable_get('scheduler_'. $node->type, 0) == 1) {
   
     $date_format = variable_get('scheduler_date_format', SCHEDULER_DATE_FORMAT);
     $internal_date_format = module_exists('date_popup') ? SCHEDULER_DATE_FORMAT : $date_format;
     
     $defaults = db_fetch_object(db_query('SELECT publish_on, unpublish_on FROM {scheduler} WHERE nid = %d', $node->nid));
   
     $form['panes']['scheduler-settings'] = array(
       '#title' => t('Scheduler'),
     );
     
     $form['panes']['scheduler-settings']['publish_on'] = array(
       '#type' => 'textfield',
       '#title' => t('Publish on'),
       '#maxlength' => 25,
       '#default_value' => isset($defaults->publish_on) && $defaults->publish_on ? format_date($defaults->publish_on, 'custom', $internal_date_format) : '',
       '#description' => t('Format: %time. Leave blank to disable scheduled publishing.', array('%time' => format_date(time(), 'custom', $date_format))),
     );
 
     $form['panes']['scheduler-settings']['unpublish_on'] = array(
       '#type' => 'textfield',
       '#title' => t('Unpublish on'),
       '#maxlength' => 25,
       '#default_value' => isset($defaults->unpublish_on) && $defaults->unpublish_on ? format_date($defaults->unpublish_on, 'custom', $internal_date_format) : '',
       '#description' => t('Format: %time. Leave blank to disable scheduled unpublishing.', array('%time' => format_date(time(), 'custom', $date_format))),
     );
   }  
   
   $form['edit'] = array(
     '#type' => 'submit',
     '#value' => t('Edit')
   );
   
   $form['delete'] = array(
     '#type' => 'submit',
     '#value' => t('Delete')
   );
   
   return $form;
 }
 
 function quickedit_nodeoptions_form_submit($form, &$form_state) {
   $nid = $form_state['values']['nid'];
   $node = $form_state['values']['node'];
   
   if ($form_state['clicked_button']['#id'] == 'edit-delete') {
     drupal_goto('node/'. $nid .'/delete');
   }
   
   $node->title = $form_state['values']['title'];
   $node->name = $form_state['values']['author'];
   $node->date = $form_state['values']['created'];
   $node->status = $form_state['values']['status'];
   $node->promote = $form_state['values']['promote'];
   $node->sticky = $form_state['values']['sticky'];
   
   if (module_exists('comment')) {
     $node->comment = $form_state['values']['comment'];
   }
   
   node_validate($node); 
   $node = node_submit($node); 
   node_save($node);  
   drupal_set_message(t('The settings have been saved'));
 }