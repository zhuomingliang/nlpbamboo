include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_source(parser.cxx)
add_source(parser_factory.cxx)
add_source(custom_parser.cxx)
add_source(crf_pos_parser.cxx)
add_source(crf_seg_parser.cxx)
add_source(ugm_seg_parser.cxx)
add_source(mfm_seg_parser.cxx)
add_source(crf_ner_nr_parser.cxx)
add_source(crf_ner_ns_parser.cxx)
add_source(crf_ner_nt_parser.cxx)
add_source(crf_ner_np_parser.cxx)
add_source(keyword_parser.cxx)

install(FILES parser_factory.hxx DESTINATION ${INC_INSTALL_DIR})
install(FILES parser.hxx DESTINATION ${INC_INSTALL_DIR})


