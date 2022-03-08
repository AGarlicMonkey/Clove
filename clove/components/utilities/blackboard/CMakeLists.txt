set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove)

#Library
add_library(
    CloveBlackboard STATIC

	${INCLUDE}/Blackboard.hpp
	${INCLUDE}/Blackboard.inl
	${SOURCE}/Blackboard.cpp
)

#Includes
target_include_directories(
    CloveBlackboard
    
	PUBLIC
		include

)

#Libraries
target_link_libraries(
    CloveBlackboard

    PUBLIC
        CloveLog
		CloveMemory
)

if(CLOVE_BUILD_TESTS)
	add_subdirectory(tests)
endif()