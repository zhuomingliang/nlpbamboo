include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(lexicon STATIC lexicon.cxx)
add_executable(lexicon_cli lexicon_cli.cxx)
target_link_libraries(lexicon_cli lexicon trie)
set_target_properties(lexicon_cli PROPERTIES OUTPUT_NAME "lexicon")
install(TARGETS lexicon_cli DESTINATION ${BIN_INSTALL_DIR})

