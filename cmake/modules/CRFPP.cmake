find_path(CRFPP_INCLUDE_DIR crfpp.h
   /usr/include
   /usr/local/include
)


foreach(libname crfpp)
	find_library(CRFPP_LIBRARIES NAMES ${libname}
	   PATHS
	   /usr/lib
	   /usr/local/lib
	)
	if (CRFPP_LIBRARIES)
		break()
	endif(CRFPP_LIBRARIES)
endforeach(libname)


if(CRFPP_INCLUDE_DIR AND CRFPP_LIBRARIES)
   set(CRFPP_FOUND TRUE)
   message(STATUS "Found libcrfpp: ${CRFPP_INCLUDE_DIR}, ${CRFPP_LIBRARIES}")
else(CRFPP_INCLUDE_DIR AND CRFPP_LIBRARIES)
   set(CRFPP_FOUND FALSE)
   message(STATUS "libcrfpp not found.")
endif(CRFPP_INCLUDE_DIR AND CRFPP_LIBRARIES)

mark_as_advanced(CRFPP_INCLUDE_DIR CRFPP_LIBRARIES)
