include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_mmap
	mmap.cxx
)
add_library(mmap STATIC ${libsrc_mmap})
