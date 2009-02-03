
set(components
	include
	utf8
	mmap
	trie
	lexicon
	config
	kea
	common
	parser
	processor
)

include_directories(${components})

macro(add_source name)
	message("Adding source ${CMAKE_CURRENT_SOURCE_DIR}/${name}")
	list(APPEND sub_sources "${CMAKE_CURRENT_SOURCE_DIR}/${name}")
endmacro(add_source)

foreach(component ${components})
	add_subdirectory(${component})
	get_directory_property(${component}_sources DIRECTORY ${component} DEFINITION sub_sources)
	list(APPEND sources ${${component}_sources})
endforeach(component)

add_library(bamboo_static STATIC bamboo.cxx ${sources})
target_link_libraries(bamboo_static dl)
set_target_properties(bamboo_static PROPERTIES OUTPUT_NAME "bamboo")

add_library(bamboo_shared SHARED bamboo.cxx)
target_link_libraries(bamboo_shared bamboo_static dl)
set_target_properties(bamboo_shared PROPERTIES OUTPUT_NAME "bamboo")
set_target_properties(bamboo_shared PROPERTIES VERSION 2)

install(TARGETS bamboo_static DESTINATION ${LIB_INSTALL_DIR})
install(TARGETS bamboo_shared DESTINATION ${LIB_INSTALL_DIR})

add_subdirectory(tools)


##################### BUILD PROCESSORS ##################################

macro(build_processor name)
	add_library(processor_${name} MODULE processor/${name}_processor.cxx
				processor/processor.cxx)
	target_link_libraries(processor_${name} -lbamboo ${ARGN})
	set_target_properties(processor_${name} PROPERTIES PREFIX "" OUTPUT_NAME ${name})
	install(TARGETS processor_${name} DESTINATION ${PROCESSOR_INSTALL_DIR})
endmacro(build_processor)

link_directories(${CMAKE_CURRENT_BINARY_DIR})

build_processor(prepare)
build_processor(maxforward)
build_processor(ugm_seg)
build_processor(single_combine)
build_processor(maxforward_combine)
build_processor(break)

find_package(CRFPP)

if (CRFPP_FOUND)
	build_processor(crf_seg -lcrfpp)
	build_processor(crf_pos -lcrfpp)
	build_processor(crf_ner_nr -lcrfpp)
	build_processor(crf_ner_ns -lcrfpp)
	build_processor(crf_ner_nt -lcrfpp)
	build_processor(crf_ner_np -lcrfpp)
	build_processor(crf_seg4ner -lcrfpp)
endif (CRFPP_FOUND)
