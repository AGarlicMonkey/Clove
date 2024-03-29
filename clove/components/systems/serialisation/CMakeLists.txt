set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Serialisation)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#CMake
find_package(yaml-cpp REQUIRED CONFIG)

#Library
add_library(
    CloveSerialisation STATIC
        ${INCLUDE}/Node.hpp
        ${INCLUDE}/Node.inl
        ${SOURCE}/Node.cpp
        ${INCLUDE}/Yaml.hpp
        ${SOURCE}/Yaml.cpp
)

#Includes
target_include_directories(
    CloveSerialisation
    
	PUBLIC
		include
)

#Libraries
target_link_libraries(
    CloveSerialisation

	PUBLIC
        CloveDefinitions
        CloveExpected
        CloveLog

    PRIVATE
        yaml-cpp::yaml-cpp
)

if(CLOVE_BUILD_TESTS)
	add_subdirectory(tests)
endif()