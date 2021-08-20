#Library
add_library(
	CloveDefinitions INTERFACE
)

#Include
target_include_directories(
	CloveDefinitions
	
	INTERFACE 
		include
)

#Preprocessor
target_compile_definitions(
	CloveDefinitions

	INTERFACE
		#Platform
		CLOVE_PLATFORM_WINDOWS=$<PLATFORM_ID:Windows>
		CLOVE_PLATFORM_LINUX=$<PLATFORM_ID:Linux>
		CLOVE_PLATFORM_MACOS=$<PLATFORM_ID:Darwin>
)