include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(parser_base STATIC parser.cxx)
add_library(bamboo_parsers STATIC parser_factory.cxx)
set (parser_links dl)
macro(build_parser name)
	add_library(parser_${name} STATIC ${name}_parser.cxx)
	target_link_libraries(parser_${name} ${ARGN} processor_factory config_finder utf8 parser_base)
	set_target_properties(parser_${name} PROPERTIES PREFIX "" OUTPUT_NAME ${name})
	set (parser_links ${parser_links} parser_${name})
endmacro(build_parser)

build_parser(custom)
build_parser(crf_seg)
build_parser(ugm_seg)
build_parser(mfm_seg)
build_parser(crf_ner_nr)
build_parser(crf_ner_ns)
build_parser(crf_ner_nt)
build_parser(keyword ycake)

target_link_libraries(bamboo_parsers ${parser_links}) 
install(FILES parser_factory.hxx DESTINATION ${INC_INSTALL_DIR})
install(FILES parser.hxx DESTINATION ${INC_INSTALL_DIR})

#add_library(bamboo_shared SHARED parser_factory.cxx)
#target_link_libraries(bamboo_shared parsers) 

#set_target_properties(bamboo_shared PROPERTIES OUTPUT_NAME "bamboo")
#set_target_properties(bamboo_shared PROPERTIES VERSION 2)

#install(TARGETS bamboo_shared DESTINATION ${LIB_INSTALL_DIR})

