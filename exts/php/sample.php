<?php
	bamboo_set("process_chain = prepare,crf2,single_combine");
	bamboo_reload();
	echo bamboo_parse("你好朋友")."\n";
?>
