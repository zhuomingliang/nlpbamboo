include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_config_parser
	lua_config.cxx
)
add_library(config_parser STATIC ${libsrc_config_parser})
add_executable(cf_tool cf_tool.cxx)
target_link_libraries(cf_tool config_parser ${LUA5_LIBRARIES})
install(TARGETS cf_tool DESTINATION ${BIN_INSTALL_DIR})
