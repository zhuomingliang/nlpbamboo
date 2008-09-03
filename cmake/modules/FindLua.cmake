find_path(LUA5_INCLUDE_DIR lua.h
   /usr/include
   /usr/include/lua
   /usr/include/lua5
   /usr/include/lua5.1
   /usr/local/include
   /usr/local/include/lua
   /usr/local/include/lua5
   /usr/local/include/lua5.1
)


foreach(libname lua lua5.0 lua5.1)
	find_library(LUA5_LIBRARIES NAMES ${libname}
	   PATHS
	   /usr/lib
	   /usr/local/lib
	)
	if (LUA5_LIBRARIES)
		break()
	endif(LUA5_LIBRARIES)
endforeach(libname)


if(LUA5_INCLUDE_DIR AND LUA5_LIBRARIES)
   set(LUA5_FOUND TRUE)
   message(STATUS "Found liblua: ${LUA5_INCLUDE_DIR}, ${LUA5_LIBRARIES}")
else(LUA5_INCLUDE_DIR AND LUA5_LIBRARIES)
   set(LUA5_FOUND FALSE)
   message(STATUS "liblua not found.")
endif(LUA5_INCLUDE_DIR AND LUA5_LIBRARIES)

mark_as_advanced(LUA5_INCLUDE_DIR LUA5_LIBRARIES)
