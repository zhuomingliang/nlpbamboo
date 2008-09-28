include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(simple_config STATIC simple_config.cxx)

add_library(config_factory STATIC config_factory.cxx)
target_link_libraries(config_factory simple_config)

add_executable(cf_tool cf_tool.cxx)
target_link_libraries(cf_tool config_factory dl)
install(TARGETS cf_tool DESTINATION ${BIN_INSTALL_DIR})
