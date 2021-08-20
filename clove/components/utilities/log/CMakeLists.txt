option(CLOVE_ENABLE_ASSERTIONS "Enables the CLOVE_ASSERT macro. Allowing the program to debug break if certain conditions fail." ON)

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Log)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    CloveLog STATIC
        ${INCLUDE}/Log.hpp
	    ${INCLUDE}/Log.inl
	    ${SOURCE}/Log.cpp
)

#Include
target_include_directories(
	CloveLog
	
	PUBLIC
		include
)

#Libraries
target_link_libraries(
    CloveLog

    PUBLIC
		CloveDefinitions
		
		CONAN_PKG::spdlog
)

#Preprocessor
target_compile_definitions(
	CloveLog

	PUBLIC
		CLOVE_ENABLE_ASSERTIONS=$<BOOL:${CLOVE_ENABLE_ASSERTIONS}>
)