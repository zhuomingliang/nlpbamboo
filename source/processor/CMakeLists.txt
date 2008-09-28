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
target_link_libraries(clxmod_unigram lexicon trie utf8 )
install(TARGETS clxmod_unigram DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_prepare MODULE prepare_processor.cxx)
target_link_libraries(clxmod_prepare lexicon trie utf8 )
install(TARGETS clxmod_prepare DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_ascii MODULE ascii_processor.cxx)
target_link_libraries(clxmod_ascii lexicon trie utf8 )
install(TARGETS clxmod_ascii DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_single_combine MODULE single_combine_processor.cxx)
target_link_libraries(clxmod_single_combine lexicon trie utf8 )
install(TARGETS clxmod_single_combine DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_combine MODULE combine_processor.cxx)
target_link_libraries(clxmod_combine lexicon trie utf8 )
install(TARGETS clxmod_combine DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_crf2 MODULE crf2_processor.cxx)
target_link_libraries(clxmod_crf2 utf8 -lcrfpp)
install(TARGETS clxmod_crf2 DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_crf MODULE crf_processor.cxx)
target_link_libraries(clxmod_crf utf8 -lcrfpp)
install(TARGETS clxmod_crf DESTINATION ${MOD_INSTALL_DIR})

add_library(clxmod_maxforward MODULE maxforward_processor.cxx)
target_link_libraries(clxmod_maxforward lexicon trie utf8 )
install(TARGETS clxmod_maxforward DESTINATION ${MOD_INSTALL_DIR})
