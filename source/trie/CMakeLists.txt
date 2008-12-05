include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_source(double_array.cxx)
add_source(datrie.cxx)

if (TRIE_DEBUG)
	add_subdirectory(debugger)
endif (TRIE_DEBUG)

set(trie_sources ${sub_sources})
add_library(trie_static STATIC ${trie_sources})

if (TRIE_SHARED)
	add_library(trie_shared SHARED ${trie_sources})
	set_target_properties(trie_shared PROPERTIES OUTPUT_NAME "trie")
	set_target_properties(trie_shared PROPERTIES VERSION 1)
endif (TRIE_SHARED)

#add_library(kvtrie STATIC kvtrie.cxx)
#add_library(kvtrie_shared MODULE kvtrie_interface.cxx)
#target_link_libraries(kvtrie_shared trie)
#set_target_properties(kvtrie_shared PROPERTIES OUTPUT_NAME "kvtrie")
#install(TARGETS kvtrie_shared DESTINATION ${LIB_INSTALL_DIR})
