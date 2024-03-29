set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/Clove/Graphics)

#CMake
option(CLOVE_ENABLE_GHA_VALIDATION "Allows for extra code to run to validate the usage of the GHA." OFF)
option(CLOVE_ENABLE_GHA_MEMORY_DEBUG "Enables logging of extra data when the GHA allocates memory." OFF)
option(CLOVE_COMPILE_DEBUG_SHADERS "Compiles shaders in debug mode, disabling optimisations." OFF)

find_package(shaderc REQUIRED CONFIG)
find_package(glslang REQUIRED CONFIG)
find_package(spirv-cross REQUIRED CONFIG)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
	find_package(Vulkan REQUIRED)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	find_library(CLOVE_METAL_LIB Metal)
	find_library(CLOVE_METALKIT_LIB MetalKit)
endif()

#Vulkan Implementation
set(
	VulkanFiles
		${INCLUDE}/Vulkan/DevicePointer.hpp
		${INCLUDE}/Vulkan/DevicePointer.inl
		${SOURCE}/Vulkan/DevicePointer.cpp
		${INCLUDE}/Vulkan/MemoryAllocator.hpp
		${INCLUDE}/Vulkan/MemoryAllocator.inl
		${SOURCE}/Vulkan/MemoryAllocator.cpp
		${INCLUDE}/Vulkan/VulkanBuffer.hpp
		${SOURCE}/Vulkan/VulkanBuffer.cpp
		${INCLUDE}/Vulkan/VulkanCommandBuffer.hpp
		${SOURCE}/Vulkan/VulkanCommandBuffer.cpp
		${INCLUDE}/Vulkan/VulkanComputeCommandBuffer.hpp
		${INCLUDE}/Vulkan/VulkanComputeCommandBuffer.inl
		${SOURCE}/Vulkan/VulkanComputeCommandBuffer.cpp
		${INCLUDE}/Vulkan/VulkanComputePipelineObject.hpp
		${SOURCE}/Vulkan/VulkanComputePipelineObject.cpp
		${INCLUDE}/Vulkan/VulkanComputeQueue.hpp
		${SOURCE}/Vulkan/VulkanComputeQueue.cpp
		${INCLUDE}/Vulkan/VulkanDescriptor.hpp
		${SOURCE}/Vulkan/VulkanDescriptor.cpp
		${INCLUDE}/Vulkan/VulkanDescriptorPool.hpp
		${INCLUDE}/Vulkan/VulkanDescriptorPool.inl
		${SOURCE}/Vulkan/VulkanDescriptorPool.cpp
		${INCLUDE}/Vulkan/VulkanDescriptorSet.hpp
		${SOURCE}/Vulkan/VulkanDescriptorSet.cpp
		${INCLUDE}/Vulkan/VulkanDescriptorSetLayout.hpp
		${INCLUDE}/Vulkan/VulkanDescriptorSetLayout.inl
		${SOURCE}/Vulkan/VulkanDescriptorSetLayout.cpp
		${INCLUDE}/Vulkan/VulkanFence.hpp
		${SOURCE}/Vulkan/VulkanFence.cpp
		${INCLUDE}/Vulkan/VulkanFramebuffer.hpp
		${SOURCE}/Vulkan/VulkanFramebuffer.cpp
		${INCLUDE}/Vulkan/VulkanGha.hpp
		${SOURCE}/Vulkan/VulkanGha.cpp
		${INCLUDE}/Vulkan/VulkanGraphicsCommandBuffer.hpp
		${INCLUDE}/Vulkan/VulkanGraphicsCommandBuffer.inl
		${SOURCE}/Vulkan/VulkanGraphicsCommandBuffer.cpp
		${INCLUDE}/Vulkan/VulkanGraphicsPipelineObject.hpp
		${SOURCE}/Vulkan/VulkanGraphicsPipelineObject.cpp
		${INCLUDE}/Vulkan/VulkanGraphicsQueue.hpp
		${SOURCE}/Vulkan/VulkanGraphicsQueue.cpp
		${INCLUDE}/Vulkan/VulkanDevice.hpp
		${SOURCE}/Vulkan/VulkanDevice.cpp
		${INCLUDE}/Vulkan/VulkanFactory.hpp
		${SOURCE}/Vulkan/VulkanFactory.cpp
		${INCLUDE}/Vulkan/VulkanResource.hpp
		${SOURCE}/Vulkan/VulkanResource.cpp
		${INCLUDE}/Vulkan/VulkanImage.hpp
		${INCLUDE}/Vulkan/VulkanImage.inl
		${SOURCE}/Vulkan/VulkanImage.cpp
		${INCLUDE}/Vulkan/VulkanImageView.hpp
		${INCLUDE}/Vulkan/VulkanImageView.inl
		${SOURCE}/Vulkan/VulkanImageView.cpp
		${INCLUDE}/Vulkan/VulkanLog.hpp
		${INCLUDE}/Vulkan/VulkanMemoryBarrier.hpp
		${SOURCE}/Vulkan/VulkanMemoryBarrier.cpp
		${INCLUDE}/Vulkan/VulkanPipelineObject.hpp
		${SOURCE}/Vulkan/VulkanPipelineObject.cpp
		${INCLUDE}/Vulkan/VulkanRenderPass.hpp
		${SOURCE}/Vulkan/VulkanRenderPass.cpp
		${INCLUDE}/Vulkan/VulkanResult.hpp
		${SOURCE}/Vulkan/VulkanResult.cpp
		${INCLUDE}/Vulkan/VulkanSampler.hpp
		${INCLUDE}/Vulkan/VulkanSampler.inl
		${SOURCE}/Vulkan/VulkanSampler.cpp
		${INCLUDE}/Vulkan/VulkanSemaphore.hpp
		${SOURCE}/Vulkan/VulkanSemaphore.cpp
		${INCLUDE}/Vulkan/VulkanShader.hpp
		${SOURCE}/Vulkan/VulkanShader.cpp
		${INCLUDE}/Vulkan/VulkanSwapchain.hpp
		${INCLUDE}/Vulkan/VulkanSwapchain.inl
		${SOURCE}/Vulkan/VulkanSwapchain.cpp
		${INCLUDE}/Vulkan/VulkanTransferCommandBuffer.hpp
		${INCLUDE}/Vulkan/VulkanTransferCommandBuffer.inl
		${SOURCE}/Vulkan/VulkanTransferCommandBuffer.cpp
		${INCLUDE}/Vulkan/VulkanTransferQueue.hpp
		${SOURCE}/Vulkan/VulkanTransferQueue.cpp
		${INCLUDE}/Vulkan/VulkanTypes.hpp
)

#Metal Implementation
set(
	MetalFiles
		${INCLUDE}/Metal/DeviceWrapper.hpp
		${INCLUDE}/Metal/MetalBuffer.hpp
		${SOURCE}/Metal/MetalBuffer.mm
		${INCLUDE}/Metal/MetalComputeCommandBuffer.hpp
		${INCLUDE}/Metal/MetalComputeCommandBuffer.inl
		${SOURCE}/Metal/MetalComputeCommandBuffer.mm
		${INCLUDE}/Metal/MetalComputePipelineObject.hpp
		${INCLUDE}/Metal/MetalComputePipelineObject.inl
		${SOURCE}/Metal/MetalComputePipelineObject.mm
		${INCLUDE}/Metal/MetalComputeQueue.hpp
		${SOURCE}/Metal/MetalComputeQueue.mm
		${INCLUDE}/Metal/MetalDescriptorPool.hpp
		${SOURCE}/Metal/MetalDescriptorPool.mm
		${INCLUDE}/Metal/MetalDescriptorSet.hpp
		${INCLUDE}/Metal/MetalDescriptorSet.inl
		${SOURCE}/Metal/MetalDescriptorSet.mm
		${INCLUDE}/Metal/MetalDescriptorSetLayout.hpp
		${INCLUDE}/Metal/MetalDescriptorSetLayout.inl
		${SOURCE}/Metal/MetalDescriptorSetLayout.mm
		${INCLUDE}/Metal/MetalDevice.hpp
		${SOURCE}/Metal/MetalDevice.mm
		${INCLUDE}/Metal/MetalFactory.hpp
		${SOURCE}/Metal/MetalFactory.mm
		${INCLUDE}/Metal/MetalFence.hpp
		${SOURCE}/Metal/MetalFence.mm
		${INCLUDE}/Metal/MetalFramebuffer.hpp
		${SOURCE}/Metal/MetalFramebuffer.mm
		${INCLUDE}/Metal/MetalGha.hpp
		${SOURCE}/Metal/MetalGha.mm
		${INCLUDE}/Metal/MetalGlobals.hpp
		${INCLUDE}/Metal/MetalGraphicsCommandBuffer.hpp
		${INCLUDE}/Metal/MetalGraphicsCommandBuffer.inl
		${SOURCE}/Metal/MetalGraphicsCommandBuffer.mm
		${INCLUDE}/Metal/MetalGraphicsPipelineObject.hpp
		${INCLUDE}/Metal/MetalGraphicsPipelineObject.inl
		${SOURCE}/Metal/MetalGraphicsPipelineObject.mm
		${INCLUDE}/Metal/MetalGraphicsQueue.hpp
		${SOURCE}/Metal/MetalGraphicsQueue.mm
		${INCLUDE}/Metal/MetalImage.hpp
		${SOURCE}/Metal/MetalImage.mm
		${INCLUDE}/Metal/MetalImageView.hpp
		${SOURCE}/Metal/MetalImageView.mm
		${INCLUDE}/Metal/MetalLog.hpp
		${INCLUDE}/Metal/MetalPipelineObject.hpp
		${SOURCE}/Metal/MetalPipelineObject.mm
		${INCLUDE}/Metal/MetalRenderPass.hpp
		${INCLUDE}/Metal/MetalRenderPass.inl
		${SOURCE}/Metal/MetalRenderPass.mm
		${INCLUDE}/Metal/MetalSampler.hpp
		${INCLUDE}/Metal/MetalSampler.inl
		${SOURCE}/Metal/MetalSampler.mm
		${INCLUDE}/Metal/MetalSemaphore.hpp
		${INCLUDE}/Metal/MetalSemaphore.inl
		${SOURCE}/Metal/MetalSemaphore.mm
		${INCLUDE}/Metal/MetalShader.hpp
		${INCLUDE}/Metal/MetalShader.inl
		${SOURCE}/Metal/MetalShader.mm
		${INCLUDE}/Metal/MetalSwapchain.hpp
		${SOURCE}/Metal/MetalSwapchain.mm
		${INCLUDE}/Metal/MetalTransferCommandBuffer.hpp
		${INCLUDE}/Metal/MetalTransferCommandBuffer.inl
		${SOURCE}/Metal/MetalTransferCommandBuffer.mm
		${INCLUDE}/Metal/MetalTransferQueue.hpp
		${SOURCE}/Metal/MetalTransferQueue.mm
		${INCLUDE}/Metal/MetalView.hpp
		${SOURCE}/Metal/MetalView.mm
)

#Validation
set(
	ValidationFiles
		${INCLUDE}/Validation/ValidationBuffer.hpp
		${INCLUDE}/Validation/ValidationBuffer.inl
		${INCLUDE}/Validation/ValidationCommandBuffer.hpp
		${INCLUDE}/Validation/ValidationCommandBuffer.inl
		${SOURCE}/Validation/ValidationCommandBuffer.cpp
		${INCLUDE}/Validation/ValidationDescriptorSet.hpp
		${INCLUDE}/Validation/ValidationDescriptorSet.inl
		${INCLUDE}/Validation/ValidationDescriptorPool.hpp
		${INCLUDE}/Validation/ValidationDescriptorPool.inl
		${INCLUDE}/Validation/ValidationLog.hpp
		${INCLUDE}/Validation/ValidationQueue.hpp
		${INCLUDE}/Validation/ValidationQueue.inl
)

#Library
add_library(
	CloveGraphics STATIC
		${INCLUDE}/Descriptor.hpp
		${INCLUDE}/Gha.hpp		
		${SOURCE}/Gha.cpp
		${INCLUDE}/GhaBuffer.hpp
		${INCLUDE}/GhaComputeCommandBuffer.hpp
		${INCLUDE}/GhaComputePipelineObject.hpp
		${INCLUDE}/GhaComputeQueue.hpp
		${INCLUDE}/GhaDescriptorPool.hpp
		${INCLUDE}/GhaDescriptorSet.hpp
		${INCLUDE}/GhaDescriptorSetLayout.hpp
		${INCLUDE}/GhaDevice.hpp
		${INCLUDE}/GhaFactory.hpp
		${INCLUDE}/GhaFence.hpp
		${INCLUDE}/GhaFramebuffer.hpp
		${INCLUDE}/GhaGraphicsCommandBuffer.hpp
		${INCLUDE}/GhaGraphicsPipelineObject.hpp
		${INCLUDE}/GhaGraphicsQueue.hpp
		${INCLUDE}/GhaImage.hpp
		${INCLUDE}/GhaImageView.hpp
		${INCLUDE}/GhaRenderPass.hpp
		${INCLUDE}/GhaSampler.hpp
		${INCLUDE}/GhaSemaphore.hpp
		${INCLUDE}/GhaShader.hpp
		${INCLUDE}/GhaSwapchain.hpp
		${INCLUDE}/GhaTransferCommandBuffer.hpp
		${INCLUDE}/GhaTransferQueue.hpp
		${INCLUDE}/GraphicsAPI.hpp
		${INCLUDE}/Helpers.hpp
		${INCLUDE}/Helpers.inl
		${INCLUDE}/MemoryBarrier.hpp
		${INCLUDE}/PipelineObject.hpp
		${INCLUDE}/Resource.hpp
		${INCLUDE}/Result.hpp
		${INCLUDE}/ShaderCompiler.hpp		
		${SOURCE}/ShaderCompiler.cpp
		
		${VulkanFiles}
		${MetalFiles}
		${ValidationFiles}

		#shaderc files required to use the file includer
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/glslc/file_includer.h
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/glslc/file_includer.cc

		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/libshaderc_util/file_finder.h
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/libshaderc_util/file_finder.cc
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/libshaderc_util/io.h
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/libshaderc_util/io.cc
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/libshaderc_util/string_piece.h
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc/libshaderc_util/universal_unistd.h
)

#Includes
target_include_directories(
    CloveGraphics
    
	PUBLIC
		include

	PRIVATE
		Vulkan::Vulkan
		${CMAKE_CURRENT_SOURCE_DIR}/external/shaderc
)

#Libraries
target_link_libraries(
    CloveGraphics
     
	PUBLIC
		CloveCast
		CloveDefinitions
		CloveExpected
		CloveLog
        CloveMaths

    PRIVATE
        shaderc::shaderc
		spirv-cross::spirv-cross

	#Windows
	PRIVATE
		$<$<PLATFORM_ID:Windows>:Vulkan::Vulkan>

	#MacOS
	PRIVATE
		$<$<PLATFORM_ID:Darwin>:${CLOVE_METAL_LIB}>
		$<$<PLATFORM_ID:Darwin>:${CLOVE_METALKIT_LIB}>

	#Linux
	PRIVATE
		$<$<PLATFORM_ID:Linux>:Vulkan::Vulkan>
)

#Compiler
target_compile_options(
	CloveGraphics

	#MacOS
	PRIVATE
		$<$<PLATFORM_ID:Darwin>:-fobjc-arc>
)

#Preprocessor
target_compile_definitions(
	CloveGraphics

	PRIVATE
		CLOVE_GHA_VALIDATION=$<BOOL:${CLOVE_ENABLE_GHA_VALIDATION}>
		CLOVE_GHA_MEMORY_DEBUG=$<BOOL:${CLOVE_ENABLE_GHA_MEMORY_DEBUG}>

		CLOVE_COMPILE_DEBUG_SHADERS=$<BOOL:${CLOVE_COMPILE_DEBUG_SHADERS}>
)

#Don't compile files not supported on certain platforms
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_source_files_properties(${MetalFiles} PROPERTIES HEADER_FILE_ONLY TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_source_files_properties(${VulkanFiles} PROPERTIES HEADER_FILE_ONLY TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set_source_files_properties(${MetalFiles} PROPERTIES HEADER_FILE_ONLY TRUE)
endif()