include(CSharpUtilities)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/resource)

add_subdirectory(components)

#Resources
set(
	RESOURCES
		${RESOURCE}/FileExplorer/png/001-folder-closed-black-shape.png
		${RESOURCE}/FileExplorer/png/002-file.png
		${RESOURCE}/FileExplorer/png/003-open-folder.png
)

#Executable
add_executable(
    Bulb
       
		${SOURCE}/Views/FileExplorer/BreadcrumbItem.xaml
		${SOURCE}/Views/FileExplorer/BreadcrumbItem.xaml.cs
		${SOURCE}/Views/FileExplorer/DirectoryTree.xaml
		${SOURCE}/Views/FileExplorer/DirectoryTree.xaml.cs
		${SOURCE}/Views/FileExplorer/DirectoryTreeItem.xaml
		${SOURCE}/Views/FileExplorer/DirectoryTreeItem.xaml.cs
		${SOURCE}/Views/FileExplorer/DirectoryView.xaml
		${SOURCE}/Views/FileExplorer/DirectoryView.xaml.cs
		${SOURCE}/Views/FileExplorer/DirectoryViewItem.xaml
		${SOURCE}/Views/FileExplorer/DirectoryViewItem.xaml.cs
		${SOURCE}/Views/FileExplorer/FileExplorer.xaml
		${SOURCE}/Views/FileExplorer/FileExplorer.xaml.cs

		${SOURCE}/Views/EntityInfo.xaml
		${SOURCE}/Views/EntityInfo.xaml.cs
		${SOURCE}/Views/MainWindow.xaml
		${SOURCE}/Views/MainWindow.xaml.cs
		${SOURCE}/Views/TypeView.xaml
		${SOURCE}/Views/TypeView.xaml.cs
		${SOURCE}/Views/Vector3Box.xaml
		${SOURCE}/Views/Vector3Box.xaml.cs
		${SOURCE}/Views/Viewport.xaml
		${SOURCE}/Views/Viewport.xaml.cs

		${SOURCE}/ViewModels/FileExplorer/DirectoryItemViewModel.cs
		${SOURCE}/ViewModels/FileExplorer/FileExplorerViewModel.cs
		${SOURCE}/ViewModels/FileExplorer/FileViewModel.cs
		${SOURCE}/ViewModels/FileExplorer/FolderViewModel.cs

		${SOURCE}/ViewModels/ComponentMenuItemViewModel.cs
		${SOURCE}/ViewModels/ComponentViewModel.cs
		${SOURCE}/ViewModels/EditorSessionViewModel.cs
		${SOURCE}/ViewModels/SceneViewModel.cs
		${SOURCE}/ViewModels/SubSystemViewModel.cs
		${SOURCE}/ViewModels/EntityViewModel.cs
		${SOURCE}/ViewModels/LogViewModel.cs
		${SOURCE}/ViewModels/TypeViewModel.cs
		
	    ${SOURCE}/AssemblyInfo.cs

		${SOURCE}/DragDropData.cs

	    ${SOURCE}/EditorApp.xaml
        ${SOURCE}/EditorApp.xaml.cs

		${SOURCE}/EditorPropertyHelper.cs
		${SOURCE}/SizePercentageConverter.cs

		${RESOURCES}
)

#Libraries
target_link_libraries(
    Bulb

	PUBLIC
		BulbUI
        BulbMembrane   
)

#Cs properties
csharp_set_designer_cs_properties(
	${SOURCE}/AssemblyInfo.cs
)

csharp_set_xaml_cs_properties(
	${SOURCE}/EditorApp.xaml
    ${SOURCE}/EditorApp.xaml.cs

	${SOURCE}/Views/FileExplorer/BreadcrumbItem.xaml
	${SOURCE}/Views/FileExplorer/BreadcrumbItem.xaml.cs

	${SOURCE}/Views/FileExplorer/DirectoryTree.xaml
	${SOURCE}/Views/FileExplorer/DirectoryTree.xaml.cs

	${SOURCE}/Views/FileExplorer/DirectoryTreeItem.xaml
	${SOURCE}/Views/FileExplorer/DirectoryTreeItem.xaml.cs

	${SOURCE}/Views/FileExplorer/DirectoryView.xaml
	${SOURCE}/Views/FileExplorer/DirectoryView.xaml.cs
	
	${SOURCE}/Views/FileExplorer/DirectoryViewItem.xaml
	${SOURCE}/Views/FileExplorer/DirectoryViewItem.xaml.cs

	${SOURCE}/Views/FileExplorer/FileExplorer.xaml
	${SOURCE}/Views/FileExplorer/FileExplorer.xaml.cs

	${SOURCE}/Views/EntityInfo.xaml
	${SOURCE}/Views/EntityInfo.xaml.cs

	${SOURCE}/Views/MainWindow.xaml
    ${SOURCE}/Views/MainWindow.xaml.cs

	${SOURCE}/Views/TypeView.xaml
    ${SOURCE}/Views/TypeView.xaml.cs

	${SOURCE}/Views/ProjectSelector.xaml
	${SOURCE}/Views/ProjectSelector.xaml.cs
	
	${SOURCE}/Views/Vector3Box.xaml
	${SOURCE}/Views/Vector3Box.xaml.cs

	${SOURCE}/Views/Viewport.xaml
	${SOURCE}/Views/Viewport.xaml.cs
)

#File properties
set_source_files_properties(${SOURCE}/EditorApp.xaml PROPERTIES VS_XAML_TYPE "ApplicationDefinition")
set_property(SOURCE ${RESOURCES} PROPERTY VS_TOOL_OVERRIDE "Resource")

#Target properties
set_target_properties(Bulb PROPERTIES
        VS_DOTNET_REFERENCES "Microsoft.CSharp;PresentationCore;PresentationFramework;System;System.Core;System.Data;System.Xaml;System.Xml;WindowsBase"
)

target_compile_options(Bulb PUBLIC "/unsafe")