include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(cnlexizer STATIC cnlexizer.cxx)

add_executable(cnlexizer_cli cnlexizer_cli.cxx)
target_link_libraries(cnlexizer_cli cnlexizer config_factory utf8 dl)
set_target_properties(cnlexizer_cli PROPERTIES OUTPUT_NAME "cnlexizer")
install(TARGETS cnlexizer_cli DESTINATION ${BIN_INSTALL_DIR})

add_library(cnlexizer_shared SHARED cnlexizer_interface.cxx)
target_link_libraries(cnlexizer_shared cnlexizer config_factory utf8 dl)
set_target_properties(cnlexizer_shared PROPERTIES OUTPUT_NAME "cnlexizer")
install(TARGETS cnlexizer_shared DESTINATION ${LIB_INSTALL_DIR})

install(FILES cnlexizer_interface.hxx DESTINATION ${INC_INSTALL_DIR})
