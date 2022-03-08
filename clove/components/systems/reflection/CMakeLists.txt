set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Reflection)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    CloveReflection STATIC

    ${INCLUDE}/Reflection.hpp
    ${INCLUDE}/Reflection.inl
    ${SOURCE}/Reflection.cpp
)

#Includes
target_include_directories(
    CloveReflection
    
	PUBLIC
		include
)


#Libraries
target_link_libraries(
    CloveReflection

    PUBLIC
        CloveDefinitions
		CloveBlackboard
)

if(CLOVE_BUILD_TESTS)
	add_subdirectory(tests)
endif()