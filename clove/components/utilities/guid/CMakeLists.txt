set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    CloveGuid STATIC
        ${INCLUDE}/Guid.hpp
        ${SOURCE}/Guid.cpp
)

#Include
target_include_directories(
	CloveGuid
	
	PUBLIC
		include
)

if(CLOVE_BUILD_TESTS)
    add_subdirectory(tests)
endif()