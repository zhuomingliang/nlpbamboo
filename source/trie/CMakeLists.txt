include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(libsrc_trie
	double_array.cxx
	datrie.cxx
)
add_library(trie STATIC ${libsrc_trie})
