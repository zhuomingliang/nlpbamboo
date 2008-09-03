include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set( libsrc_debugger
	trie_debugger.cxx
)
add_library(debugger STATIC ${libsrc_debugger})
