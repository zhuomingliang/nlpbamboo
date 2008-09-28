include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_processor
	ascii_processor.cxx
	prepare_processor.cxx
	maxforward_processor.cxx
	unigram_processor.cxx
	crf_processor.cxx
	crf2_processor.cxx
	combine_processor.cxx
	single_combine_processor.cxx
)
add_library(clxmod_unigram MODULE unigram_processor.cxx)
target_link_libraries(clxmod_unigram lexicon trie utf8 ${LUA5_LIBRARIES})
install(TARGETS clxmod_unigram DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_ascii MODULE ascii_processor.cxx)
target_link_libraries(clxmod_ascii lexicon trie utf8 ${LUA5_LIBRARIES})
install(TARGETS clxmod_ascii DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_single_combine MODULE single_combine_processor.cxx)
target_link_libraries(clxmod_single_combine lexicon trie utf8 ${LUA5_LIBRARIES})
install(TARGETS clxmod_single_combine DESTINATION ${MOD_INSTALL_DIR})

