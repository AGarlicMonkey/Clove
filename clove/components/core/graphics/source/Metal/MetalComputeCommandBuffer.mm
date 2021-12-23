#include "Clove/Graphics/Metal/MetalComputeCommandBuffer.hpp"

#include "Clove/Graphics/Metal/MetalBuffer.hpp"
#include "Clove/Graphics/Metal/MetalComputePipelineObject.hpp"
#include "Clove/Graphics/Metal/MetalDescriptorSet.hpp"
#include "Clove/Graphics/Metal/MetalGlobals.hpp"
#include "Clove/Graphics/Metal/MetalImage.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"
#include "Clove/Graphics/Metal/MetalShader.hpp"

#include <Clove/Cast.hpp>

namespace clove {
	MetalComputeCommandBuffer::MetalComputeCommandBuffer() = default;
	
	MetalComputeCommandBuffer::MetalComputeCommandBuffer(MetalComputeCommandBuffer &&other) noexcept = default;
	
	MetalComputeCommandBuffer &MetalComputeCommandBuffer::operator=(MetalComputeCommandBuffer &&other) noexcept = default;
	
	MetalComputeCommandBuffer::~MetalComputeCommandBuffer() = default;

	void MetalComputeCommandBuffer::beginRecording(CommandBufferUsage usageFlag) {
		commands.clear();
	}
	
	void MetalComputeCommandBuffer::endRecording() {
		//no op
	}

	void MetalComputeCommandBuffer::bindPipelineObject(GhaComputePipelineObject &pipelineObject) {
        auto *const metalPipelineObject{ polyCast<MetalComputePipelineObject>(&pipelineObject) };
        activePipeline = metalPipelineObject;
        
		commands.emplace_back([metalPipelineObject](id<MTLComputeCommandEncoder> encoder) {
			[encoder setComputePipelineState:metalPipelineObject->getPipelineState()];
		});
	}

	void MetalComputeCommandBuffer::bindDescriptorSet(GhaDescriptorSet &descriptorSet, uint32_t const setNum) {
		commands.emplace_back([descriptorSet = &descriptorSet, setNum](id<MTLComputeCommandEncoder> encoder) {
            auto const *const metalDescriptorSet{ polyCast<MetalDescriptorSet>(descriptorSet) };
            if(metalDescriptorSet == nullptr) {
                CLOVE_LOG(CloveGhaMetal, LogLevel::Error, "{0}: DescriptorSet is nullptr", CLOVE_FUNCTION_NAME);
                return;
            }
            
            id<MTLBuffer> backingBuffer{ metalDescriptorSet->getBackingBuffer() };
            std::optional<size_t> computeOffset{ metalDescriptorSet->getComputeOffset() };
            CLOVE_ASSERT(computeOffset.has_value());
            
            [encoder setBuffer:backingBuffer
                        offset:computeOffset.value()
                       atIndex:setNum];
		});
	}
	
	void MetalComputeCommandBuffer::pushConstant(size_t const offset, size_t const size, void const *data) {
		class Functor{
			//VARIABLES
		private:
			size_t size{};
			std::unique_ptr<std::byte> data{};
			
			//FUNCTIONS
		public:
			Functor() = delete;
			Functor(size_t size, void const *data)
				: size{ size }
				, data{ reinterpret_cast<std::byte*>(malloc(size)) } {
				memcpy(this->data.get(), data, size);
			}
			
			Functor(Functor const &other)
				: size{ other.size } {
				data = std::unique_ptr<std::byte>{ reinterpret_cast<std::byte*>(malloc(size)) };
				memcpy(data.get(), other.data.get(), size);
			}
			Functor(Functor &&other) = default;
			
			Functor &operator=(Functor const &other) {
				size = other.size;
				data = std::unique_ptr<std::byte>{ reinterpret_cast<std::byte*>(malloc(size)) };
				memcpy(data.get(), other.data.get(), size);
			}
			Functor &operator=(Functor &&other) = default;
			
			~Functor() = default;
			
			void operator()(id<MTLComputeCommandEncoder> encoder) {
				[encoder setBytes:data.get()
						   length:size
						  atIndex:pushConstantSlot];
			}
		};
		
		commands.emplace_back(Functor{ size, data });
	}

	void MetalComputeCommandBuffer::disptach(vec3ui groupCount) {
        auto const *const metalShader{ polyCast<MetalShader const>(activePipeline->getDescriptor().shader) };
        vec3ui const &workGroupSize{ metalShader->getWorkgroupSize() };
        
		commands.emplace_back([workGroupSize, groupCount](id<MTLComputeCommandEncoder> encoder){
			[encoder dispatchThreadgroups:MTLSizeMake(groupCount.x, groupCount.y, groupCount.z)
					threadsPerThreadgroup:MTLSizeMake(workGroupSize.x, workGroupSize.y, workGroupSize.z)];
		});
	}

	void MetalComputeCommandBuffer::bufferMemoryBarrier(GhaBuffer &buffer, BufferMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
		commands.emplace_back([buffer = &buffer, sourceStage, destinationStage](id<MTLComputeCommandEncoder> encoder){
			id<MTLBuffer> mtlBuffer{ polyCast<MetalBuffer>(buffer)->getBuffer() };
			[encoder memoryBarrierWithResources:&mtlBuffer
										  count:1];
		});
	}
	
	void MetalComputeCommandBuffer::imageMemoryBarrier(GhaImage &image, ImageMemoryBarrierInfo const &barrierInfo, PipelineStage sourceStage, PipelineStage destinationStage) {
		commands.emplace_back([image = &image, sourceStage, destinationStage](id<MTLComputeCommandEncoder> encoder){
			id<MTLTexture> mtlTexture{ polyCast<MetalImage>(image)->getTexture() };
			[encoder memoryBarrierWithResources:&mtlTexture
										  count:1];
		});
	}
}
