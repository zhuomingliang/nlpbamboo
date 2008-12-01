include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(ycake_src
	text_parser.cxx
	ycake_mmap.cxx
	ycake_doc.cxx
	segment_tool.cxx
	token_dict.cxx
	token_aff_dict.cxx
	token_filter.cxx
	ranker.cxx
	prepare_ranker.cxx
	tfidf_ranker.cxx
	graph_ranker.cxx
	udgraph.cxx
	ycake_hash.cxx
	ycake.cxx
)
add_library(ycake STATIC ${ycake_src})
target_link_libraries(ycake trie lexicon processor_factory utf8 config_factory)

#add_executable(ycake_word_train word_train.cxx)
#target_link_libraries(ycake_word_train ycake bamboo parser_impl config_factory utf8 trie dl)
#install(TARGETS ycake_word_train DESTINATION ${BIN_INSTALL_DIR})

#add_executable(ycake_wordaff_train word_aff_train.cxx)
#target_link_libraries(ycake_wordaff_train ycake bamboo parser_impl config_factory utf8 trie dl)
#install(TARGETS ycake_wordaff_train DESTINATION ${BIN_INSTALL_DIR})
