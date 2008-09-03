include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_processor
	unigram_processor.cxx
)
add_library(processor STATIC ${libsrc_processor})
