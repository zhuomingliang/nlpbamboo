SET(cnlexizer_tools
	pdc_normalize
	pdc_tool
	crf_tool
	ngm_tool
	switch_column
	make_turing_data
	auto_build
	update_user_combine
	)
install(FILES build_settings DESTINATION ${ETC_INSTALL_DIR})
install(PROGRAMS ${cnlexizer_tools} DESTINATION ${BIN_INSTALL_DIR})
