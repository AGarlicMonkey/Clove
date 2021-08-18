#Library
add_library(
    CloveExpected INTERFACE
)

#Include
target_include_directories(
	CloveExpected
	
	INTERFACE 
		include
)

if(CLOVE_BUILD_TESTS)
    add_subdirectory(tests)
endif()