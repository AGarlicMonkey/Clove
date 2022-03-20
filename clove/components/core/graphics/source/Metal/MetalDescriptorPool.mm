#include "Clove/Graphics/Metal/MetalDescriptorPool.hpp"

#include "Clove/Graphics/Helpers.hpp"
#include "Clove/Graphics/Metal/MetalDescriptorSet.hpp"
#include "Clove/Graphics/Metal/MetalDescriptorSetLayout.hpp"
#include "Clove/Graphics/Metal/MetalLog.hpp"

#include <Clove/Cast.hpp>
#include <optional>

namespace clove {
    MetalDescriptorPool::BufferPool::BufferPool() = default;
    
    MetalDescriptorPool::BufferPool::~BufferPool() = default;
    
    id<MTLBuffer> MetalDescriptorPool::BufferPool::allocateBuffer(id<MTLDevice> device, size_t size) {
        BufferEntry *foundEntry{ nullptr };
        
        for(BufferEntry &entry : buffers[size]){
            if(entry.isFree) {
                foundEntry = &entry;
                break;
            }
        }
        
        if(foundEntry == nullptr) {
            buffers[size].push_back({});
            foundEntry = &buffers[size].back();
            
            foundEntry->buffer = [device newBufferWithLength:size options:0];
            
            static uint32_t bufferCount{ 1 };
            foundEntry->buffer.label = [NSString stringWithFormat:@"Descriptor pool buffer %i", bufferCount++];
        }
        
        foundEntry->isFree = false;
        return foundEntry->buffer;
    }
    
    void MetalDescriptorPool::BufferPool::freeBuffer(id<MTLBuffer> buffer) {
        for(auto &&[size, bufferVector] : buffers) {
            for(BufferEntry &entry : bufferVector) {
                if(entry.buffer == buffer){
                    entry.isFree = true;
                    break;
                }
            }
        }
    }
    
    void MetalDescriptorPool::BufferPool::reset() {
        for(auto &&[size, bufferVector] : buffers) {
            for(BufferEntry &entry : bufferVector) {
                entry.isFree = true;
            }
        }
    }
    
    MetalDescriptorPool::MetalDescriptorPool(Descriptor descriptor, id<MTLDevice> device)
        : descriptor{ std::move(descriptor) }
        , device{ device } {
    }
    
    MetalDescriptorPool::MetalDescriptorPool(MetalDescriptorPool &&other) noexcept = default;
    
    MetalDescriptorPool &MetalDescriptorPool::operator=(MetalDescriptorPool &&other) noexcept = default;
    
    MetalDescriptorPool::~MetalDescriptorPool() = default;
    
    GhaDescriptorPool::Descriptor const &MetalDescriptorPool::getDescriptor() const {
        return descriptor;
    }
    
    std::unique_ptr<GhaDescriptorSet> MetalDescriptorPool::allocateDescriptorSets(GhaDescriptorSetLayout const *const layout) {
        return std::move(allocateDescriptorSets(std::vector{ layout })[0]);
    }
    
    std::vector<std::unique_ptr<GhaDescriptorSet>> MetalDescriptorPool::allocateDescriptorSets(std::vector<GhaDescriptorSetLayout const *> const &layouts) {
        @autoreleasepool{
            size_t const setNum{ layouts.size() };
            
            std::vector<MetalDescriptorSetLayout const *> metalDescriptorSetLayouts{};
            metalDescriptorSetLayouts.reserve(setNum);
            for(size_t i{ 0 }; i < layouts.size(); ++i) {
                metalDescriptorSetLayouts.push_back(polyCast<MetalDescriptorSetLayout const>(layouts[i]));
            }
            
            auto const createEncoder = [this](NSArray<MTLArgumentDescriptor *> *descriptors, size_t &totalBackingBufferSize, size_t &currentOffset){
                size_t constexpr minOffsetAlignment{ 256 }; //Offsets into argument buffers must be a multiple of 256
                
                std::optional<MetalDescriptorSet::ArgumentEncoder> ret{};
                if(descriptors.count > 0) {
                    MetalDescriptorSet::ArgumentEncoder encoder{};
                    
                    encoder.encoder = [device newArgumentEncoderWithArguments:descriptors];
                    
                    size_t const alignmentOffset{ currentOffset % minOffsetAlignment }; //Calculate the remainder of the offset so we know how far away we are from the next boundary
                    size_t const alignment{ alignmentOffset != 0 ? (minOffsetAlignment - alignmentOffset) : 0 };
                    
                    currentOffset += alignment;
                    encoder.offset = currentOffset;
                    
                    size_t const bufferSize{ encoder.encoder.encodedLength };
                    
                    totalBackingBufferSize += currentOffset + bufferSize;
                    currentOffset = totalBackingBufferSize;

                    ret = encoder;
                }
                return ret;
            };
            
            std::vector<std::unique_ptr<GhaDescriptorSet>> descriptorSets{};
            descriptorSets.reserve(setNum);
            for(size_t i{ 0 }; i < layouts.size(); ++i) {
                size_t totalBackingBufferSize{ 0 };
                size_t currentOffset{ 0 };
                
                std::optional<MetalDescriptorSet::ArgumentEncoder> vertexEncoder{ createEncoder(metalDescriptorSetLayouts[i]->getVertexDescriptors(), totalBackingBufferSize, currentOffset) };
                std::optional<MetalDescriptorSet::ArgumentEncoder> pixelEncoder{ createEncoder(metalDescriptorSetLayouts[i]->getPixelDescriptors(), totalBackingBufferSize, currentOffset) };
                std::optional<MetalDescriptorSet::ArgumentEncoder> computeEncoder{ createEncoder(metalDescriptorSetLayouts[i]->getComputeDescriptors(), totalBackingBufferSize, currentOffset) };
                
                CLOVE_ASSERT(totalBackingBufferSize != 0);
                id<MTLBuffer> encoderBuffer{ bufferPool.allocateBuffer(device, totalBackingBufferSize) };
                
                if(vertexEncoder.has_value()) {
                    [vertexEncoder->encoder setArgumentBuffer:encoderBuffer offset:vertexEncoder->offset];
                }
                if(pixelEncoder.has_value()) {
                    [pixelEncoder->encoder setArgumentBuffer:encoderBuffer offset:pixelEncoder->offset];
                }
                if(computeEncoder.has_value()) {
                    [computeEncoder->encoder setArgumentBuffer:encoderBuffer offset:computeEncoder->offset];
                }
                
                descriptorSets.emplace_back(createGhaObject<MetalDescriptorSet>(std::move(vertexEncoder), std::move(pixelEncoder), std::move(computeEncoder), encoderBuffer, layouts[i]));
            }
            
            return descriptorSets;
        }
    }
    
    void MetalDescriptorPool::freeDescriptorSets(std::unique_ptr<GhaDescriptorSet> &descriptorSet) {
        auto *const mtlDescriptorSet{ polyCast<MetalDescriptorSet>(descriptorSet.get()) };
        if(mtlDescriptorSet == nullptr) {
            CLOVE_LOG(CloveGhaMetal, LogLevel::Warning, "{0}: Descriptor set provided is nullptr. Buffers might never be freed", CLOVE_FUNCTION_NAME);
            return;
        }

        freeBuffer(*mtlDescriptorSet);

        descriptorSet.reset();
    }
    
    void MetalDescriptorPool::freeDescriptorSets(std::vector<std::unique_ptr<GhaDescriptorSet>> &descriptorSets) {
        for(auto &descriptorSet : descriptorSets) {
            auto *const mtlDescriptorSet{ polyCast<MetalDescriptorSet>(descriptorSet.get()) };
            if(mtlDescriptorSet == nullptr) {
                CLOVE_LOG(CloveGhaMetal, LogLevel::Warning, "{0}: Descriptor set provided is nullptr. Buffers might never be freed", CLOVE_FUNCTION_NAME);
                continue;
            }

            freeBuffer(*mtlDescriptorSet);

            descriptorSet.reset();
        }
    }
    
    void MetalDescriptorPool::reset() {
        bufferPool.reset();
    }

    void MetalDescriptorPool::freeBuffer(MetalDescriptorSet &descriptorSet) {
        bufferPool.freeBuffer(descriptorSet.getBackingBuffer());
    }
}
