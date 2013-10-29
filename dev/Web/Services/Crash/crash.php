<?php

	$platform = $_GET["type"];
	$filename = $_GET["name"];
	$filepath = "./reports/".$platform."/".$filename;	
	$report_data = file_get_contents('php://input');
	
	$fp = fopen($filepath, 'w');
	fwrite($fp, $report_data);
	fclose($fp);

?>