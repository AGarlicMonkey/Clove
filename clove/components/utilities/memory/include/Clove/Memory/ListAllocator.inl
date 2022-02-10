namespace clove {
    size_t ListAllocator::getSize() const {
        return backingMemorySize;
    }

    std::byte const *ListAllocator::getMemory() const {
        return backingMemory;
    }

    template<typename T>
    T *ListAllocator::alloc() {
        return reinterpret_cast<T *>(alloc(sizeof(T), alignof(T)));
    }
}