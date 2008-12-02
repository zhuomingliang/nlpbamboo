include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bamboo_cli bamboo_cli.cxx)
if (CLI_SHARED)
	target_link_libraries(bamboo_cli -lbamboo)
else (CLI_SHARED)
	target_link_libraries(bamboo_cli parser dl)
endif (CLI_SHARED)
set_target_properties(bamboo_cli PROPERTIES OUTPUT_NAME "bamboo")
install(TARGETS bamboo_cli DESTINATION ${BIN_INSTALL_DIR})

