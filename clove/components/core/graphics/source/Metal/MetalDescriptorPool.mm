#include "Clove/Graphics/Metal/MetalDescriptorPool.hpp"

#include "Clove/Graphics/Metal/MetalDescriptorSet.hpp"
#include "Clove/Graphics/Metal/MetalDescriptorSetLayout.hpp"

#include <Clove/Cast.hpp>

namespace garlic::clove {
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
    
    std::shared_ptr<GhaDescriptorSet> MetalDescriptorPool::allocateDescriptorSets(std::shared_ptr<GhaDescriptorSetLayout> const &layout) {
        return allocateDescriptorSets(std::vector{ layout })[0];
    }
    
    std::vector<std::shared_ptr<GhaDescriptorSet>> MetalDescriptorPool::allocateDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSetLayout>> const &layouts) {
        @autoreleasepool{
            size_t const setNum{ layouts.size() };
            
            std::vector<MetalDescriptorSetLayout *> metalDescriptorSets{};
            metalDescriptorSets.reserve(setNum);
            for(size_t i{ 0 }; i < layouts.size(); ++i) {
                metalDescriptorSets.emplace_back(polyCast<MetalDescriptorSetLayout>(layouts[i].get()));
            }
            
            std::vector<std::shared_ptr<GhaDescriptorSet>> descriptorSets{};
            descriptorSets.reserve(setNum);
            for(size_t i{ 0 }; i < layouts.size(); ++i) {
                id<MTLArgumentEncoder> vertexEncoder{ nullptr };
                id<MTLBuffer> vertexEncoderBuffer{ nullptr };
                if(metalDescriptorSets[i]->getVertexDescriptors().count > 0){
                    vertexEncoder       = [device newArgumentEncoderWithArguments:metalDescriptorSets[i]->getVertexDescriptors()];
                    vertexEncoderBuffer = bufferPool.allocateBuffer(device, vertexEncoder.encodedLength);
                    
                    [vertexEncoder setArgumentBuffer:vertexEncoderBuffer offset:0];
                }
                
                id<MTLArgumentEncoder> pixelEncoder{ nullptr };
                id<MTLBuffer> pixelEncoderBuffer{ nullptr };
                if(metalDescriptorSets[i]->getPixelDescriptors().count > 0){
                    pixelEncoder       = [device newArgumentEncoderWithArguments:metalDescriptorSets[i]->getPixelDescriptors()];
                    pixelEncoderBuffer = bufferPool.allocateBuffer(device, pixelEncoder.encodedLength);
                    
                    [pixelEncoder setArgumentBuffer:pixelEncoderBuffer offset:0];
                }
                
                id<MTLArgumentEncoder> computeEncoder{ nullptr };
                id<MTLBuffer> computeEncoderBuffer{ nullptr };
                if(metalDescriptorSets[i]->getComputeDescriptors().count > 0){
                    computeEncoder       = [device newArgumentEncoderWithArguments:metalDescriptorSets[i]->getComputeDescriptors()];
                    computeEncoderBuffer = bufferPool.allocateBuffer(device, computeEncoder.encodedLength);
                    
                    [computeEncoder setArgumentBuffer:computeEncoderBuffer offset:0];
                }
                
                descriptorSets.emplace_back(std::make_shared<MetalDescriptorSet>(vertexEncoder, vertexEncoderBuffer, pixelEncoder, pixelEncoderBuffer, computeEncoder, computeEncoderBuffer, layouts[i]));
            }
            
            return descriptorSets;
        }
    }
    
    void MetalDescriptorPool::freeDescriptorSets(std::shared_ptr<GhaDescriptorSet> const &descriptorSet) {
        freeDescriptorSets(std::vector{ descriptorSet });
    }
    
    void MetalDescriptorPool::freeDescriptorSets(std::vector<std::shared_ptr<GhaDescriptorSet>> const &descriptorSets) {
        for(auto const &descriptorSet : descriptorSets) {
            auto *mtlDescriptorSet{ polyCast<MetalDescriptorSet>(descriptorSet.get()) };
            
            if(id<MTLBuffer> vertexBuffer{ mtlDescriptorSet->getVertexBuffer() }) {
                bufferPool.freeBuffer(vertexBuffer);
            }
            if(id<MTLBuffer> pixelBuffer{ mtlDescriptorSet->getPixelBuffer() }) {
                bufferPool.freeBuffer(pixelBuffer);
            }
            if(id<MTLBuffer> computeBuffer{ mtlDescriptorSet->getComputeBuffer() }) {
                bufferPool.freeBuffer(computeBuffer);
            }
        }
    }
    
    void MetalDescriptorPool::reset() {
        bufferPool.reset();
    }
}
