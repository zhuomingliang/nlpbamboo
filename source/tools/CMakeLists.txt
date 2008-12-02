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
	pdc_normalize_4nr
	)

macro(build_training_tool name)
	add_executable(${name} ${name}.cxx)
	target_link_libraries(${name} ${ARGN})
	install(TARGETS ${name} DESTINATION ${BIN_INSTALL_DIR})
endmacro(build_training_tool)

find_package(CRFPP)
if (CRFPP_FOUND)
	build_training_tool(crf2_tool utf8)
	build_training_tool(ner_nr_tool utf8)
	build_training_tool(ner_tool utf8)
endif (CRFPP_FOUND)

install(FILES build_settings DESTINATION ${ETC_INSTALL_DIR})
install(PROGRAMS ${tools} DESTINATION ${BIN_INSTALL_DIR})
