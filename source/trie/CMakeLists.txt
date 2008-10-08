include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(libsrc_trie
	double_array.cxx
	datrie.cxx
)
add_library(trie STATIC ${libsrc_trie})
add_library(kvtrie STATIC kvtrie.cxx)

add_library(kvtrie_shared MODULE kvtrie_interface.cxx)
target_link_libraries(kvtrie_shared trie)
set_target_properties(kvtrie_shared PROPERTIES OUTPUT_NAME "kvtrie")
install(TARGETS kvtrie_shared DESTINATION ${LIB_INSTALL_DIR})
