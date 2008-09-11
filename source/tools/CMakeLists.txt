SET(cnlexizer_tools
	pdc_normalize
	pdc_tool
	ngm_tool
	switch_column
	make_turing_data
	auto_build
	)
install(PROGRAMS ${cnlexizer_tools} DESTINATION ${BIN_INSTALL_DIR})
