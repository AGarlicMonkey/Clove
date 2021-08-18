set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Event)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    CloveEvent STATIC
        ${INCLUDE}/EventContainerBase.hpp
        ${SOURCE}/EventContainerBase.cpp
	    ${INCLUDE}/EventDispatcher.hpp
        ${INCLUDE}/EventDispatcher.inl
        ${SOURCE}/EventDispatcher.cpp
        ${INCLUDE}/EventHandle.hpp
        ${SOURCE}/EventHandle.cpp
	    ${INCLUDE}/EventManager.hpp
        ${INCLUDE}/EventManager.inl
        ${SOURCE}/EventManager.cpp
	    ${INCLUDE}/EventTypes.hpp
)

#Include
target_include_directories(
	CloveEvent 
	
	PUBLIC
		include
)

if(CLOVE_BUILD_TESTS)
	add_subdirectory(tests)
endif()