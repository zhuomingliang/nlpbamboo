include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_source(lexicon.cxx)

add_library(lexicon_static ${sub_sources})
