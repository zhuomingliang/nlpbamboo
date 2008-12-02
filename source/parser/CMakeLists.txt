include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(parser STATIC parser_factory.cxx)

macro(build_parser name)
	add_library(parser_${name} STATIC ${name}_parser.cxx)
	target_link_libraries(parser_${name} ${ARGN} processor_factory config_finder utf8)
	set_target_properties(parser_${name} PROPERTIES PREFIX "" OUTPUT_NAME ${name})
	target_link_libraries(parser parser_${name})
endmacro(build_parser)

build_parser(custom)
build_parser(crf_seg)
build_parser(ugm_seg)
build_parser(mfm_seg)
build_parser(crf_ner_nr)
build_parser(crf_ner_ns)
build_parser(crf_ner_nt)
build_parser(keyword ycake)

