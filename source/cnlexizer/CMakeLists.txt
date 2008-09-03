include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_cnlexizer_core
	cnlexizer.cxx
)
add_library(cnlexizer_core STATIC ${libsrc_cnlexizer_core})

add_executable(cnlexizer_cli cli.cxx)
target_link_libraries(cnlexizer_cli cnlexizer_core lexicon trie config_parser utf8 ${LUA5_LIBRARIES})

add_library(cnlexizer SHARED cnlexizer_interface.cxx)
target_link_libraries(cnlexizer cnlexizer_core lexicon trie config_parser utf8 ${LUA5_LIBRARIES})

install(TARGETS cnlexizer_cli DESTINATION ${BIN_INSTALL_DIR})
install(TARGETS cnlexizer DESTINATION ${LIB_INSTALL_DIR})
install(FILES cnlexizer_interface.hxx DESTINATION ${INC_INSTALL_DIR})
