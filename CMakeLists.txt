project(bamboo)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_INSTALL_PREFIX "/opt/bamboo")

set(LIB_INSTALL_DIR "lib")
set(BIN_INSTALL_DIR "bin")
set(ETC_INSTALL_DIR "etc")
set(TRAINING_INSTALL_DIR "data")
set(INC_INSTALL_DIR "include")
set(PROCESSOR_INSTALL_DIR "processor")
set(EXT_INSTALL_DIR "exts")

set(CMAKE_CXX_FLAGS "-fPIC -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g ")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG")

foreach(subdir cmake source etc training exts)

	if ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )
		add_subdirectory( ${subdir} )
	endif ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )

endforeach(subdir)

