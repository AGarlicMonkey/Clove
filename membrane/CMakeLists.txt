set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Membrane)

#Library
add_library(
    Membrane SHARED
        ${INCLUDE}/Application.hpp
        ${SOURCE}/Application.cpp
        
        ${INCLUDE}/EditorSubSystem.hpp
        ${SOURCE}/EditorSubSystem.cpp
        
        ${INCLUDE}/EditorVFS.hpp
        ${INCLUDE}/EditorVFS.inl
        ${SOURCE}/EditorVFS.cpp

        ${INCLUDE}/FileSystemHelpers.hpp
        ${SOURCE}/FileSystemHelpers.cpp

        ${INCLUDE}/Log.hpp
        ${SOURCE}/Log.cpp
        ${INCLUDE}/MembraneLog.hpp

        ${INCLUDE}/MessageBase.hpp
        ${INCLUDE}/MessageHandler.hpp
        ${SOURCE}/MessageHandler.cpp
        ${INCLUDE}/Messages.hpp

        ${INCLUDE}/NameComponent.hpp

        ${INCLUDE}/ReflectionHelpers.hpp
        ${SOURCE}/ReflectionHelpers.cpp

        ${INCLUDE}/RuntimeSubSystem.hpp
        ${SOURCE}/RuntimeSubSystem.cpp
)

#Include
target_include_directories(
    Membrane

    PUBLIC
        include
)

#Libraries
target_link_libraries(
    Membrane

    PRIVATE
        Clove
)

#PreProcessor
target_compile_definitions(
    Membrane

    PRIVATE
        ROOT_DIR="${CMAKE_SOURCE_DIR}"   
)

#Properties
set_target_properties(
    Membrane
    
    PROPERTIES 
        COMMON_LANGUAGE_RUNTIME ""
        VS_DOTNET_REFERENCES "System;PresentationCore;PresentationFramework;WindowsBase"
        CXX_STANDARD 17
)