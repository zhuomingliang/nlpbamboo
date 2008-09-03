include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_lexicon
	lexicon.cxx
)
add_library(lexicon STATIC ${libsrc_lexicon})
add_executable(lexicon_cli lexicon_cli.cxx)
target_link_libraries(lexicon_cli lexicon trie)
install(TARGETS lexicon_cli DESTINATION ${BIN_INSTALL_DIR})

