SET(cnlexizer_tools
	pdc_normalize
	pdc_tool
	crf_tool
	ngm_tool
	switch_column
	make_turing_data
	auto_build
	)
add_executable(qil qil.cxx)
target_link_libraries(qil trie ${LUA5_LIBRARIES})

install(PROGRAMS ${cnlexizer_tools} DESTINATION ${BIN_INSTALL_DIR})
install(TARGETS qil DESTINATION ${BIN_INSTALL_DIR})
