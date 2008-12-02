SET(tools
	pdc_normalize
	pdc_tool
	crf_tool
	ngm_tool
	switch_column
	make_turing_data
	auto_build
	update_user_combine
	convert_pos
	pdc_pos_tool
	)
find_package(CRFPP)
if (CRFPP_FOUND)
	add_executable(crf2_tool crf2_tool.cxx)
	target_link_libraries(crf2_tool utf8)
	install(TARGETS crf2_tool DESTINATION ${BIN_INSTALL_DIR})
endif (CRFPP_FOUND)

install(FILES build_settings DESTINATION ${ETC_INSTALL_DIR})
install(PROGRAMS ${tools} DESTINATION ${BIN_INSTALL_DIR})
