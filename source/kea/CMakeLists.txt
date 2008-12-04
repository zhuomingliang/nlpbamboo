include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(kea_src
	text_parser.cxx
	kea_mmap.cxx
	kea_doc.cxx
	segment_tool.cxx
	token_dict.cxx
	token_aff_dict.cxx
	token_filter.cxx
	ranker.cxx
	prepare_ranker.cxx
	tfidf_ranker.cxx
	graph_ranker.cxx
	udgraph.cxx
	kea_hash.cxx
	kea.cxx
)
add_library(kea STATIC ${kea_src})
target_link_libraries(kea trie lexicon processor_factory utf8 config_factory)

add_executable(kea_word_train word_train.cxx)
target_link_libraries(kea_word_train kea bamboo_static config_factory utf8 trie dl)
install(TARGETS kea_word_train DESTINATION ${BIN_INSTALL_DIR})

add_executable(kea_wordaff_train word_aff_train.cxx)
target_link_libraries(kea_wordaff_train kea bamboo_static config_factory utf8 trie dl)
install(TARGETS kea_wordaff_train DESTINATION ${BIN_INSTALL_DIR})
