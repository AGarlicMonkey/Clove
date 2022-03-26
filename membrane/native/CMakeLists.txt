set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Membrane)

#Library
add_library(
    MembraneNative SHARED
        ${INCLUDE}/Application.hpp
        ${SOURCE}/Application.cpp

        ${INCLUDE}/EditorSubSystem.hpp
        ${SOURCE}/EditorSubSystem.cpp

        ${INCLUDE}/EditorVFS.hpp
        ${INCLUDE}/EditorVFS.inl
        ${SOURCE}/EditorVFS.cpp

        ${INCLUDE}/Export.hpp

        ${INCLUDE}/FileSystem.hpp
        ${SOURCE}/FileSystem.cpp
        #${INCLUDE}/Log.hpp
        #${SOURCE}/Log.cpp
        ${INCLUDE}/Reflection.hpp
        ${SOURCE}/Reflection.cpp

        ${INCLUDE}/NameComponent.hpp
)

#Include
target_include_directories(
    MembraneNative

    PUBLIC
        include
)

#Libraries
target_link_libraries(
    MembraneNative

    PRIVATE
        Clove
)

#PreProcessor
target_compile_definitions(
    MembraneNative

    PRIVATE
        ROOT_DIR="${CMAKE_SOURCE_DIR}"   
)

target_compile_options(
    MembraneNative

    PRIVATE
        /EHs
)