#pragma once

#include "Clove/Graphics/GhaDescriptorSet.hpp"

#include "Clove/Graphics/GhaShader.hpp"

#include <unordered_map>
#include <MetalKit/MetalKit.h>
#include <optional>

namespace clove {
	class GhaDescriptorSetLayout;
}

namespace clove {
	class MetalDescriptorSet : public GhaDescriptorSet {
        //TYPES
    public:
        struct ArgumentEncoder {
            id<MTLArgumentEncoder> encoder{ nullptr };  /**< Encoder object to write commands into. */
            size_t offset{ 0 };                         /**< Offset into the backing buffer. */
        };
        
		//VARIABLES
	private:
        std::optional<ArgumentEncoder> vertexEncoder{};
        std::optional<ArgumentEncoder> pixelEncoder{};
        std::optional<ArgumentEncoder> computeEncoder{};
        
        id<MTLBuffer> backingBuffer{ nullptr }; /**< Backing buffer for all encoder objects. */

		GhaDescriptorSetLayout const *layout{ nullptr };
		
		//FUNCTIONS
	public:
		MetalDescriptorSet() = delete;
		MetalDescriptorSet(std::optional<ArgumentEncoder> vertexEncoder, std::optional<ArgumentEncoder> pixelEncoder, std::optional<ArgumentEncoder> computeEncoder, id<MTLBuffer> backingBuffer, GhaDescriptorSetLayout const *layout);
		
		MetalDescriptorSet(MetalDescriptorSet const &other) = delete;
		MetalDescriptorSet(MetalDescriptorSet &&other) noexcept;
		
		MetalDescriptorSet &operator=(MetalDescriptorSet const &other) = delete;
		MetalDescriptorSet &operator=(MetalDescriptorSet &&other) noexcept;
		
		~MetalDescriptorSet();

        void write(GhaBuffer const &buffer, size_t const offset, size_t const range, DescriptorType const descriptorType, uint32_t const bindingSlot) override;

        void write(GhaImageView const &imageView, GhaImage::Layout const layout, DescriptorType const descriptorType, uint32_t const bindingSlot) override;

        void write(GhaSampler const &sampler, uint32_t const bindingSlot) override;

        inline std::optional<size_t> getVertexOffset() const;
		inline std::optional<size_t> getPixelOffset() const;
		inline std::optional<size_t> getComputeOffset() const;
        
        inline id<MTLBuffer> getBackingBuffer() const;
		
	private:
		GhaShader::Stage getStageFromBindingSlot(uint32_t const bindingSlot);
	};
}

#include "MetalDescriptorSet.inl"
