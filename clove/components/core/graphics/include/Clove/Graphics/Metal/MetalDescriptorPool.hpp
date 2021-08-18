#pragma once

#include "Clove/Graphics/GhaDescriptorPool.hpp"

#include <MetalKit/MetalKit.h>
#include <unordered_map>
#include <vector>

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

		std::shared_ptr<GhaDescriptorSet> allocateDescriptorSets(std::shared_ptr<GhaDescriptorSetLayout> const &layout) override;
		std::vector<std::shared_ptr<GhaDescriptorSet>> allocateDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSetLayout>> const &layouts) override;

		void freeDescriptorSets(std::shared_ptr<GhaDescriptorSet> const &descriptorSet) override;
		void freeDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSet>> const &descriptorSets) override;

		void reset() override;
	};
}
