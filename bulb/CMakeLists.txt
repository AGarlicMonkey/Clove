include(CSharpUtilities)

#Create another project to enable C#
project(Garlic-Editor LANGUAGES C CXX CSharp)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)

#Executable
add_executable(
    Bulb
	    ${SOURCE}/EditorApp.xaml
        ${SOURCE}/EditorApp.xaml.cs

	    ${SOURCE}/AssemblyInfo.cs
        
	    ${SOURCE}/MainWindow.xaml
		${SOURCE}/MainWindow.xaml.cs
		
		${SOURCE}/ViewModel.cs
		${SOURCE}/MainWindowViewModel.cs
		
		${SOURCE}/EditorLogger.cs
)

#Libraries
target_link_libraries(
    Bulb

    PUBLIC
        Membrane   
)

#Cs properties
csharp_set_designer_cs_properties(
	${SOURCE}/AssemblyInfo.cs
)

csharp_set_xaml_cs_properties(
	${SOURCE}/EditorApp.xaml
    ${SOURCE}/EditorApp.xaml.cs
    
	${SOURCE}/MainWindow.xaml
    ${SOURCE}/MainWindow.xaml.cs
)

#File properties
set_source_files_properties(${SOURCE}/EditorApp.xaml PROPERTIES VS_XAML_TYPE "ApplicationDefinition")

#Target properties
set_target_properties(Bulb 
	PROPERTIES 
		VS_DOTNET_FRAMEWORK_VERSION "4.8"
        VS_DOTNET_REFERENCES "Microsoft.CSharp;PresentationCore;PresentationFramework;System;System.Core;System.Data;System.Xaml;System.Xml;WindowsBase"
)

#Set the output directory of OpenAL
set_target_properties(OpenAL PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})