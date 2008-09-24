project(lexize)

cmake_minimum_required(VERSION 2.6)

set(LIB_INSTALL_DIR "/lib")
set(BIN_INSTALL_DIR "/bin")
set(ETC_INSTALL_DIR "/etc")
set(TRAINING_INSTALL_DIR "/training")
set(INC_INSTALL_DIR "/include")

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-pg -O2 -fPIC -fno-reorder-blocks -fno-schedule-insns -fno-inline")
#set(CMAKE_CXX_FLAGS_DEBUG "-pg -O2")

foreach(subdir cmake source etc training)

	if ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )
		add_subdirectory( ${subdir} )
	endif ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )

endforeach(subdir)
