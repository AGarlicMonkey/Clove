set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Memory)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
	CloveMemory STATIC
		${INCLUDE}/AllocatorStrategy.hpp
        ${INCLUDE}/ListAllocator.hpp
		${SOURCE}/ListAllocator.cpp
		${INCLUDE}/PoolAllocator.hpp
		${INCLUDE}/PoolAllocator.inl
		${INCLUDE}/StackAllocator.hpp
		${SOURCE}/StackAllocator.cpp
)

#Include
target_include_directories(
	CloveMemory
	
	PUBLIC
		include
)

#Libraries
target_link_libraries(
    CloveMemory

	PUBLIC #Public because PoolAllocator.inl includes the headers
		CloveDefinitions
		CloveLog
)

if(CLOVE_BUILD_TESTS)
	add_subdirectory(tests)
endif()