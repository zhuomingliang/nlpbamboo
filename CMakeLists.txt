project(cnlexizer)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_INSTALL_PREFIX "/opt/cnlexizer")

set(LIB_INSTALL_DIR "lib")
set(BIN_INSTALL_DIR "bin")
set(ETC_INSTALL_DIR "etc")
set(TRAINING_INSTALL_DIR "data")
set(INC_INSTALL_DIR "include")
set(PROCESSOR_INSTALL_DIR "processor")

set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -fPIC")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG -fPIC")

foreach(subdir cmake source etc training)

	if ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )
		add_subdirectory( ${subdir} )
	endif ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} )

endforeach(subdir)
