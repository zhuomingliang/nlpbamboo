find_path(CRFPP_INCLUDE_DIR crfpp.h
	HINTS
	PATHS
	/usr/local/include
	/usr/include
	/opt/local
	/opt
)
find_library(CRFPP_LIBRARY NAMES crfpp
	HINTS   
	PATHS
	PATH_SUFFIXES lib64 lib
	/usr/local/lib
	/usr/lib
	/opt/local
	/opt
)

if(CRFPP_INCLUDE_DIR AND CRFPP_LIBRARY)
   set(CRFPP_FOUND 1)
   message(STATUS "CRF++ found: ${CRFPP_INCLUDE_DIR} ${CRFPP_LIBRARY}")
else (CRFPP_INCLUDE_DIR AND CRFPP_LIBRARY)
   message(STATUS "CRF++ not found")
endif(CRFPP_INCLUDE_DIR AND CRFPP_LIBRARY)

mark_as_advanced(CRFPP_INCLUDE_DIR CRFPP_LIBRARY)
