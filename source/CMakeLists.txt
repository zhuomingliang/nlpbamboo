set(components
	utf8
	mmap
	trie
	lexicon
	config
	processor
	debugger
	cnlexizer
	tools
)

include_directories(${components})

foreach(component ${components})
	add_subdirectory( ${component} )
endforeach(component)

