include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(bamboo STATIC bamboo.cxx)
target_link_libraries(bamboo bamboo_parsers)

add_library(bamboo_shared SHARED bamboo.cxx)
target_link_libraries(bamboo_shared bamboo_parsers)

set_target_properties(bamboo_shared PROPERTIES OUTPUT_NAME "bamboo")
set_target_properties(bamboo_shared PROPERTIES VERSION 2)

install(TARGETS bamboo_shared DESTINATION ${LIB_INSTALL_DIR})
install(FILES bamboo.hxx DESTINATION ${INC_INSTALL_DIR})

