project(lexize_source)

set(LEXIZE_COMPONENT
	utf8
	mmap
	trie
	lexicon
	config
	debugger
	cnlexizer
	tools
)

include_directories(${LUA5_INCLUDE_DIR} ${LEXIZE_COMPONENT})

foreach(component ${LEXIZE_COMPONENT})
	add_subdirectory( ${component} )
endforeach(component)

