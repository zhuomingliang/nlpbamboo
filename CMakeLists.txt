project(lexize)

cmake_minimum_required(VERSION 2.6)

set(LIB_INSTALL_DIR "/lib")
set(BIN_INSTALL_DIR "/bin")
set(ETC_INSTALL_DIR "/etc")
set(TRAINING_INSTALL_DIR "/training")
set(INC_INSTALL_DIR "/include")
set(PROCESSOR_INSTALL_DIR "/processor")
set(CONFIG_PARSER_INSTALL_DIR "/config_parser")

set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -fPIC")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG -fPIC")

foreach(subdir cmake source etc training)

	if ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )
		add_subdirectory( ${subdir} )
	endif ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )

endforeach(subdir)
