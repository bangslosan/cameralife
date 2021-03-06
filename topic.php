<?php
/**
* Displays all albums for a given topic
* @author Will Entriken <cameralife@phor.net>
* @copyright Copyright (c) 2001-2009 Will Entriken
* @access public
*/

$features=array('database','theme');
require 'main.inc';

$topic = new Topic($_GET['name']);

$count = array_sum($topic->GetCounts());
if (!isset($_GET['edit']) && $count == 0) {
  header("HTTP/1.0 404 Not Found");
  $cameralife->Error("This folder does not exist.");
}

$topic->ShowPage();
