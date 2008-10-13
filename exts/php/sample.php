<?php
	bamboo_set("process_chain = prepare,crf2,single_combine");
	bamboo_reload();
	echo bamboo_parse("你好朋友")."\n";
	bamboo_set("process_chain = prepare,crf2,single_combine,pos");
	bamboo_reload();
	echo bamboo_parse_with_pos("你好朋友")."\n";
?>
