include_directories(${CMAKE_CURRENT_SOURCE_DIR})

macro(build_parser name)
	add_library(parser_${name} STATIC ${name}_parser.cxx)
	target_link_libraries(parser_${name} ${ARGN} processor_factory)
	set_target_properties(parser_${name} PROPERTIES PREFIX "" OUTPUT_NAME ${name})
endmacro(build_parser)

build_parser(custom)
build_parser(crf_seg)

