include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(bamboo_static STATIC bamboo.cxx)
target_link_libraries(bamboo_static bamboo_parsers)

set_target_properties(bamboo_static PROPERTIES OUTPUT_NAME "bamboo")
#set_target_properties(bamboo_static PROPERTIES VERSION 2)

add_library(bamboo_shared SHARED bamboo.cxx)
target_link_libraries(bamboo_shared bamboo_parsers)

set_target_properties(bamboo_shared PROPERTIES OUTPUT_NAME "bamboo")
set_target_properties(bamboo_shared PROPERTIES VERSION 2)

install(TARGETS bamboo_static DESTINATION ${LIB_INSTALL_DIR})
install(TARGETS bamboo_shared DESTINATION ${LIB_INSTALL_DIR})
install(FILES bamboo.hxx DESTINATION ${INC_INSTALL_DIR})

