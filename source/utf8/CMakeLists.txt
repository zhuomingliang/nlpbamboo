include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_utf8
	utf8.cxx
)
add_library(utf8 STATIC ${libsrc_utf8})
