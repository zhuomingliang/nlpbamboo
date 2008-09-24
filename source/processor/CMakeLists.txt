include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_processor
	ascii_processor.cxx
	maxforward_processor.cxx
	unigram_processor.cxx
	crf_processor.cxx
	crf2_processor.cxx
	combine_processor.cxx
	single_combine_processor.cxx
)
add_library(processor STATIC ${libsrc_processor})
target_link_libraries(processor -lcrfpp)
