file(GLOB cmake_modules "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake")
foreach(module ${cmake_modules})
	include (${module})
endforeach(module)
