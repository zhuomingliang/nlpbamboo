include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_source(liblexicon.cxx)

add_library(lexicon_static ${sub_sources})
