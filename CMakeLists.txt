cmake_minimum_required(VERSION 3.18.0)

enable_testing()

project(
	Clove 
	VERSION 0.5.0
	LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")

#Project wide settings
option(CLOVE_BUILD_BULB "Build Bulb, Clove's editor" ON)
option(CLOVE_BUILD_TESTS "Build Clove's tests" OFF)
option(CLOVE_MANUALLY_INSTALL_PACKAGES "Stop CMake automatically installing conan packages when configuring" OFF)

#Required for linking shared object (*.so) files on Linux
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#Set up conan
if(NOT CLOVE_MANUALLY_INSTALL_PACKAGES)
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
	list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

	if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
 		message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   		file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.17/conan.cmake"
        	          "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
	endif()
	include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

	get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
	if(${isMultiConfig})
		foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
	    	conan_cmake_autodetect(settings 
								   BUILD_TYPE ${TYPE})
    		conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_CURRENT_SOURCE_DIR}
                        		BUILD missing
                        		REMOTE conancenter
                        		SETTINGS ${settings})
		endforeach()
	else()
		conan_cmake_autodetect(settings)
		conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_CURRENT_SOURCE_DIR}
        		            BUILD missing
                		    REMOTE conancenter
                    		SETTINGS ${settings})
	endif()	
else()
	#Assuming packages have been installed into the root binary dir
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
	list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
endif()

#Engine
add_subdirectory(clove)

#Editor
if(CLOVE_BUILD_BULB AND MSVC)
	add_subdirectory(bulb)
endif()