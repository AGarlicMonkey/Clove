namespace clove {
    template<typename T>
    T *StackAllocator::alloc() {
        return reinterpret_cast<T *>(alloc(sizeof(T), alignof(T)));
    }
}