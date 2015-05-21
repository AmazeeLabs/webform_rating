<?php

/**
 * The node rating page.
 */

$page = new stdClass();
$page->disabled = FALSE; /* Edit this to true to make a default page disabled initially */
$page->api_version = 1;
$page->name = 'node_rating';
$page->task = 'page';
$page->admin_title = 'Node Rating';
$page->admin_description = 'A page that will print a webform in the context of a node which will be rated.';
$page->path = 'node/%target_node/rate/%webform_node';
$page->access = array(
  'plugins' => array(
    0 => array(
      'name' => 'webform_rating_rate_access',
      'settings' => NULL,
      'context' => array(
        0 => 'argument_entity_id:node_1',
        1 => 'argument_entity_id:node_2',
      ),
      'not' => FALSE,
    ),
  ),
  'logic' => 'and',
);
$page->menu = array();
$page->arguments = array(
  'target_node' => array(
    'id' => 1,
    'identifier' => 'Target Node',
    'name' => 'entity_id:node',
    'settings' => array(),
  ),
  'webform_node' => array(
    'id' => 2,
    'identifier' => 'Webform Node',
    'name' => 'entity_id:node',
    'settings' => array(),
  ),
);
$page->conf = array(
  'admin_paths' => FALSE,
);
$page->default_handlers = array();
$handler = new stdClass();
$handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
$handler->api_version = 1;
$handler->name = 'page_node_rating__panel_context_a74c6180-e1e4-41b9-99fc-691dfd50e6e2';
$handler->task = 'page';
$handler->subtask = 'node_rating';
$handler->handler = 'panel_context';
$handler->weight = 0;
$handler->conf = array(
  'title' => 'Panel',
  'no_blocks' => 0,
  'pipeline' => 'standard',
  'body_classes_to_remove' => '',
  'body_classes_to_add' => '',
  'css_id' => '',
  'css' => '',
  'contexts' => array(),
  'relationships' => array(),
);
$display = new panels_display();
$display->layout = 'flexible';
$display->layout_settings = array();
$display->panel_settings = array(
  'style_settings' => array(
    'default' => NULL,
    'center' => NULL,
  ),
);
$display->cache = array();
$display->title = '';
$display->uuid = 'ed97c88e-cd3f-423b-b74e-f327aa66dbcf';
$display->content = array();
$display->panels = array();
  $pane = new stdClass();
  $pane->pid = 'new-5025dce4-2bf7-4500-a885-e1624f636247';
  $pane->panel = 'center';
  $pane->type = 'entity_field_extra';
  $pane->subtype = 'node:webform';
  $pane->shown = TRUE;
  $pane->access = array();
  $pane->configuration = array(
    'view_mode' => 'full',
    'context' => 'argument_entity_id:node_2',
    'override_title' => 0,
    'override_title_text' => '',
  );
  $pane->cache = array();
  $pane->style = array(
    'settings' => NULL,
  );
  $pane->css = array();
  $pane->extras = array();
  $pane->position = 0;
  $pane->locks = array();
  $pane->uuid = '5025dce4-2bf7-4500-a885-e1624f636247';
  $display->content['new-5025dce4-2bf7-4500-a885-e1624f636247'] = $pane;
  $display->panels['center'][0] = 'new-5025dce4-2bf7-4500-a885-e1624f636247';
$display->hide_title = PANELS_TITLE_FIXED;
$display->title_pane = 'new-5025dce4-2bf7-4500-a885-e1624f636247';
$handler->conf['display'] = $display;
$page->default_handlers[$handler->name] = $handler;
