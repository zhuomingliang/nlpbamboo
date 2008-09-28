include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(simple_config STATIC simple_config.cxx)

add_library(config_factory STATIC config_factory.cxx)
target_link_libraries(config_factory simple_config)

add_executable(config_parser config_parser.cxx)
target_link_libraries(config_parser config_factory dl)
install(TARGETS config_parser DESTINATION ${BIN_INSTALL_DIR})
