include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_source(utf8.cxx)

add_library(utf8_static STATIC utf8.cxx)
