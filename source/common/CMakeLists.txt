include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(token STATIC token_impl.cxx)
add_library(config_finder STATIC config_finder.cxx)

install(FILES token.hxx DESTINATION ${INC_INSTALL_DIR})
install(FILES bamboo_defs.h DESTINATION ${INC_INSTALL_DIR})
