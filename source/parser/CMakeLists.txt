include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(custom_parser STATIC custom_parser.cxx)
target_link_libraries(custom_parser processor_factory)
