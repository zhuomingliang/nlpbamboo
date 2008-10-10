include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(parser_impl STATIC parser_impl.cxx)
add_library(bamboo STATIC bamboo.cxx)

add_library(bamboo_shared SHARED bamboo.cxx)
target_link_libraries(bamboo_shared parser_impl config_factory utf8 dl)
set_target_properties(bamboo_shared PROPERTIES OUTPUT_NAME "bamboo")
install(TARGETS bamboo_shared DESTINATION ${LIB_INSTALL_DIR})
install(FILES bamboo.hxx DESTINATION ${INC_INSTALL_DIR})

add_executable(bamboo_cli cli.cxx)
target_link_libraries(bamboo_cli -lbamboo)
set_target_properties(bamboo_cli PROPERTIES OUTPUT_NAME "bamboo")
install(TARGETS bamboo_cli DESTINATION ${BIN_INSTALL_DIR})

