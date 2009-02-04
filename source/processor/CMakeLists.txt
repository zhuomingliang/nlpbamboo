include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_source(processor_factory.cxx)

# We'll link utf8 and trie statically into correspoinding processors
# The reason is that when libbamboo.so or some shared library which is linked
# with libbamboo.so is open by `dlopen' without RTLD_NOW | RTLD_GLBOAL flag,
# the symbols of utf8 and trie are not loaded into its process space.
# Therefore, you will see the 'undefined symbol' notice when it turns out \
# to load processors.

macro(build_processor name)
	add_library(processor_${name} MODULE ${name}_processor.cxx processor.cxx)
	target_link_libraries(processor_${name} trie_static utf8_static ${ARGN})
	set_target_properties(processor_${name} PROPERTIES PREFIX "" OUTPUT_NAME ${name})
	install(TARGETS processor_${name} DESTINATION ${PROCESSOR_INSTALL_DIR})
endmacro(build_processor)

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
