set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Maths)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    CloveMaths INTERFACE
)

#Include
target_include_directories(
	CloveMaths
	
	INTERFACE 
		include
)