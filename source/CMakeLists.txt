
set(components
	include
	utf8
	mmap
	trie
	lexicon
	config
	processor
	processor/experiment
	kea
	common
	parser
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
#install(TARGETS bamboo_shared DESTINATION ${LIB_INSTALL_DIR})

add_subdirectory(tools)
