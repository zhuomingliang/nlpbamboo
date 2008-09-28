SET(cnlexizer_tools
	pdc_normalize
	pdc_tool
	crf_tool
	ngm_tool
	switch_column
	make_turing_data
	auto_build
	)
add_executable(kvtrie_cli kvtrie_cli.cxx)
add_executable(qil qil.cxx)
target_link_libraries(qil trie)
target_link_libraries(kvtrie_cli kvtrie trie)
set_target_properties(kvtrie_cli PROPERTIES OUTPUT_NAME "kvtrie")

install(PROGRAMS ${cnlexizer_tools} DESTINATION ${BIN_INSTALL_DIR})
install(TARGETS qil kvtrie_cli DESTINATION ${BIN_INSTALL_DIR})
