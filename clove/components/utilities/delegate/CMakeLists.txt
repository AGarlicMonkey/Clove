set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Delegate)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    CloveDelegate STATIC
        ${INCLUDE}/DelegateHandle.hpp
	    ${SOURCE}/DelegateHandle.cpp
	    ${INCLUDE}/MultiCastDelegate.hpp
	    ${INCLUDE}/MultiCastDelegate.inl
	    ${INCLUDE}/SingleCastDelegate.hpp
	    ${INCLUDE}/SingleCastDelegate.inl
)

#Include
target_include_directories(
	CloveDelegate
	
	PUBLIC
		include
)

if(CLOVE_BUILD_TESTS)
	add_subdirectory(tests)
endif()