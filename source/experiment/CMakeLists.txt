add_executable(bamboo_foo foo.cxx)
target_link_libraries(bamboo_foo bamboo parser_impl config_factory utf8 dl)
set_target_properties(bamboo_foo PROPERTIES OUTPUT_NAME "foo")
install(TARGETS bamboo_foo DESTINATION ${EXP_INSTALL_DIR})
