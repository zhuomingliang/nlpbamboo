include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_source(simple_config.cxx)
add_source(config_factory.cxx)

add_library(config_static ${sub_sources})
