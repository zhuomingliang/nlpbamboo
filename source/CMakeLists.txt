set(components
	utf8
	mmap
	trie
	lexicon
	config
	processor
	processor/experiment
	debugger
#	bamboo
	tools
	kea
	common
	parser
	interface
	cli
)

include_directories(${components})

foreach(component ${components})
	add_subdirectory( ${component} )
endforeach(component)

