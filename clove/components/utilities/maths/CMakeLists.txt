#CMake
find_package(glm REQUIRED CONFIG)

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

#Libraries
target_link_libraries(
	CloveMaths

	INTERFACE
		glm::glm
)

#Definitions
target_compile_definitions( 
	CloveMaths 
 
	INTERFACE 
		GLM_FORCE_RADIANS 
		GLM_FORCE_LEFT_HANDED 
		GLM_FORCE_DEPTH_ZERO_TO_ONE 
)