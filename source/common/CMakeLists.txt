include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_source(token_impl.cxx)
add_source(config_finder.cxx)

install(FILES token.hxx DESTINATION ${INC_INSTALL_DIR})
