namespace clv::gfx::vk {
    VkDeviceSize MemoryAllocator::Block::getSize() const {
        return size;
    }

    uint32_t MemoryAllocator::Block::getMemoryTypeIndex() const {
        return memoryTypeIndex;
    }
}