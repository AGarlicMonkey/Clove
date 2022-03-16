# Clove

The goal of Clove is to create an extremely modular engine where it's libraries can be used independently.

Please note that the engine is still in very early development and so the API will fluctuate wildly and without warning!

The Clove respository contains the following projects:

|Project|Language|Description|
|-|-|-|
|Clove|C++|Clove is the engine itself. This is where all the platform / graphics / rendering / systems etc. are.|
|Membrane|C++/C#|Membrane acts as a middle man between Clove and Bulb. Providing a C# API of Clove.|
|Bulb|C#|Bulb is Clove's editor. Allowing for an easy way to create scenes for Clove.|

Note: Bulb is only currently supported on Windows.

Currently, **Clove** supports the following platforms:

|**Platform**|**API**|
|-|-|
|Windows|Vulkan|
|Linux|Vulkan|
|MacOS|Metal|

## Building

Clove is built using CMake.  
Inside the project directory, use the following commands to generate and build the project:
```
mkdir build
cd build
cmake ..
cmake --build .
```
