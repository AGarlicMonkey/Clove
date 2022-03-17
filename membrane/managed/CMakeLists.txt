set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    MembraneManaged SHARED

    ${SOURCE}/Application.cs
)

#Libraries
target_link_libraries(
    MembraneManaged

    PRIVATE
        MembraneNative
)