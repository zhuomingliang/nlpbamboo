#!/usr/bin/env php
<?php

function filter($tok) {
	if(preg_match("/[0-9\-]+\/m/", $tok)) {
		return true;
	}
}

function get_pos($tok, &$str, &$pos) {
	if($tok[0]=='[') {
		$tok = substr($tok, 1);
	}
	if(strrpos($tok, ']')) {
		$tok =  substr($tok, 0, strrpos($tok, ']'));
	}
	if(preg_match("/([^\/]+)\/([^\/]+)/", $tok, $res)) {
		$str = $res[1];
		$pos = $res[2];
		return true;
	}
	return false;
}

function generate($corpus, $output) {
	$h = fopen($corpus, "r");
	if(!$h) return;

	$ho = fopen($output, "w");

	while (!feof($h)) {
		$line = fgets($h);
		if(strlen($line)==0) continue;

		$cnt=0;
		$tok = strtok($line, " \n\t");
		while ($tok !== false) {
			if($cnt++==0 && filter($tok)==true) {
				$tok = strtok(" \n\t");
				continue;
			}
			$flag = get_pos($tok, $str, $pos);
			if($flag==true) {
				fwrite($ho, "$str $pos\n");
			}
			$tok = strtok(" \n\t");
		}
		fwrite($ho, "\n");
	}
	fclose($ho);
	fclose($h);
}

/*
 * Convert People Daily Corpus for Part-Of-Speech training
 */
if($argc != 3) {
	echo "Usage: ./pdc_pos.php corpus_file output_file\n";
	exit(1);
}

$corpus = $argv[1];
$output = $argv[2];

generate($corpus, $output);
?>
