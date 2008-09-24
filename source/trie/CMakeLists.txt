include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(libsrc_trie
	double_array.cxx
	datrie.cxx
)
add_library(trie STATIC ${libsrc_trie})
add_library(kvtrie STATIC kvtrie.cxx)

add_library(clx_kvtrie MODULE kvtrie_interface.cxx)
target_link_libraries(clx_kvtrie trie)
install(TARGETS clx_kvtrie DESTINATION ${LIB_INSTALL_DIR})
