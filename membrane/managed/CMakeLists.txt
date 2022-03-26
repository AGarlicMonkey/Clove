set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Library
add_library(
    MembraneManaged SHARED

    ${SOURCE}/Application.cs
    ${SOURCE}/FileSystem.cs
    #${SOURCE}/Log.cs
    ${SOURCE}/Reflection.cs
)

#Libraries
target_link_libraries(
    MembraneManaged

    PRIVATE
        MembraneNative
)

#Target properties
set_target_properties(MembraneManaged PROPERTIES
        VS_DOTNET_REFERENCES "Microsoft.CSharp;System;System.Core;System.Data"
)