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

macro(build_tool name)
	add_executable(${name} ${name}.cxx)
	target_link_libraries(${name} ${ARGN})
	install(TARGETS ${name} DESTINATION ${BIN_INSTALL_DIR})
endmacro(build_tool)

find_package(CRFPP)

build_tool(bamboo bamboo_static)
build_tool(config config_static)
build_tool(lexicon lexicon_static trie_static)
build_tool(word_train bamboo_static)
build_tool(word_aff_train bamboo_static)
if (CRFPP_FOUND)
	build_tool(crf2_tool utf8_static)
	build_tool(ner_nr_tool utf8_static)
	build_tool(ner_tool utf8_static)
endif (CRFPP_FOUND)

install(FILES build_settings DESTINATION ${ETC_INSTALL_DIR})
install(PROGRAMS ${tools} DESTINATION ${BIN_INSTALL_DIR})
