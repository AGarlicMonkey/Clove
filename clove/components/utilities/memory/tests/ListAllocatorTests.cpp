#include <Clove/Memory/ListAllocator.hpp>
#include <array>
#include <gtest/gtest.h>

using namespace clove;

TEST(ListAllocatorTests, CanAllocateAnAmountOfBytes) {
    const size_t allocatorSize = 256 * 1024;//256kb
    ListAllocator allocator(allocatorSize);

    void* data = allocator.alloc(100);

    EXPECT_TRUE(data != nullptr);
}

TEST(ListAllocatorTests, CannotAllocateMoreThanTheAllocatorHas) {
    const size_t allocatorSize = 256 * 1024;//256kb
    ListAllocator allocator(allocatorSize);

    void* data = allocator.alloc(allocatorSize + 1);

    EXPECT_FALSE(data != nullptr);
}

TEST(ListAllocatorTests, CanFreeAndReallocateBytes) {
    const size_t allocatorSize = 256 * 1024;//256kb
    ListAllocator allocator(allocatorSize);

    void* allData = allocator.alloc(allocatorSize);

    EXPECT_TRUE(allData != nullptr);

    allocator.free(allData);

    void* someData = allocator.alloc(500);

    EXPECT_TRUE(someData != nullptr);
}

TEST(ListAllocatorTests, CanInitialiseWithARangeOfMemory) {
    std::array<std::byte, 256 * 1024> allocatorRange;
    ListAllocator allocator(allocatorRange.data(), allocatorRange.size());

    void* data = allocator.alloc(100);

    EXPECT_TRUE(data != nullptr);
}

TEST(ListAllocatorTests, FreeWillFreeAndReallocTheCorrectRange) {
    const size_t allocatorSize = 256 * 1024;//256kb
    ListAllocator allocator(allocatorSize);

    const uint32_t value = 5;

    uint32_t* first = reinterpret_cast<uint32_t*>(allocator.alloc(sizeof(uint32_t)));
    *first          = value;

    allocator.free(first);

    uint32_t* second = reinterpret_cast<uint32_t*>(allocator.alloc(sizeof(uint32_t)));

    EXPECT_EQ(value, *second);
}