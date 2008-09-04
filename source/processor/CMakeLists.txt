include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_processor
	ascii_processor.cxx
	maxforward_processor.cxx
	unigram_processor.cxx
	combine_processor.cxx
)
add_library(processor STATIC ${libsrc_processor})
