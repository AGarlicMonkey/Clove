#pragma once

#include "Clove/Graphics/GhaDescriptorPool.hpp"

#include <MetalKit/MetalKit.h>
#include <unordered_map>
#include <vector>

namespace clove {
    class MetalDescriptorSet;
}

namespace clove {
	class MetalDescriptorPool : public GhaDescriptorPool {
		//TYPES
	private:
		class BufferPool {
			//TYPES
		private:
			struct BufferEntry {
				bool isFree{ true };
				id<MTLBuffer> buffer{ nullptr };
			};
			
			//VARIABLES
		private:
			std::unordered_map<size_t, std::vector<BufferEntry>> buffers{};
			
			//FUNCTIONS
		public:
			BufferPool();
			~BufferPool();
			
			id<MTLBuffer> allocateBuffer(id<MTLDevice> device, size_t size);
			void freeBuffer(id<MTLBuffer> buffer);
			
			void reset();
		};
		
		//VARIABLES
	private:
		Descriptor descriptor{};
		
		id<MTLDevice> device{ nullptr };
		
		BufferPool bufferPool{};
		
		//FUNCTIONS
	public:
		MetalDescriptorPool() = delete;
		MetalDescriptorPool(Descriptor descriptor, id<MTLDevice> device);
		
		MetalDescriptorPool(MetalDescriptorPool const &other) = delete;
		MetalDescriptorPool(MetalDescriptorPool &&other) noexcept;
		
		MetalDescriptorPool &operator=(MetalDescriptorPool const &other) = delete;
		MetalDescriptorPool &operator=(MetalDescriptorPool &&other) noexcept;
		
		~MetalDescriptorPool();
		
		Descriptor const &getDescriptor() const override;

		std::unique_ptr<GhaDescriptorSet> allocateDescriptorSets(GhaDescriptorSetLayout const *const layout) override;
		std::vector<std::unique_ptr<GhaDescriptorSet>> allocateDescriptorSets(std::vector<GhaDescriptorSetLayout const *> const &layouts) override;

        void freeDescriptorSets(std::unique_ptr<GhaDescriptorSet> &descriptorSet) override;
        void freeDescriptorSets(std::vector<std::unique_ptr<GhaDescriptorSet>> &descriptorSets) override;

        void reset() override;

	private:
        void freeBuffers(MetalDescriptorSet &descriptorSet);
    };
}
