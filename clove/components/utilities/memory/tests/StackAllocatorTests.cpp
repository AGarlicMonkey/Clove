#include <Clove/Memory/StackAllocator.hpp>
#include <array>
#include <gtest/gtest.h>

using namespace clove;

TEST(StackAllocatorTests, CanAllocateAnAmountOfBytes) {
    const size_t allocatorSize = 256 * 1024;//256kb
    StackAllocator allocator(allocatorSize);

    void *data = allocator.alloc(100);

    EXPECT_TRUE(data != nullptr);
}

TEST(StackAllocatorTests, CannotAllocateMoreThanTheAllocatorHas) {
    const size_t allocatorSize = 256 * 1024;//256kb
    StackAllocator allocator(allocatorSize);

    void *data = allocator.alloc(allocatorSize + 1);

    EXPECT_FALSE(data != nullptr);
}

TEST(StackAllocatorTests, CanFreeAndReallocateBytes) {
    const size_t allocatorSize = 256 * 1024;//256kb
    StackAllocator allocator(allocatorSize);

    void *allData = allocator.alloc(allocatorSize);

    EXPECT_TRUE(allData != nullptr);

    allocator.free();

    void *someData = allocator.alloc(500);

    EXPECT_TRUE(someData != nullptr);
}

TEST(StackAllocatorTests, CanInitialiseWithARangeOfMemory) {
    std::array<std::byte, 256 * 1024> allocatorRange;
    StackAllocator allocator(allocatorRange.data(), allocatorRange.size());

    void *data = allocator.alloc(100);

    EXPECT_TRUE(data != nullptr);
}

TEST(StackAllocatorTests, CanFreeToAMarker) {
    const size_t allocatorSize = 256 * 1024;//256kb
    StackAllocator allocator(allocatorSize);

    float *a = reinterpret_cast<float *>(allocator.alloc(sizeof(float)));

    StackAllocator::Marker marker = allocator.markPosition();

    float *b = reinterpret_cast<float *>(allocator.alloc(sizeof(float)));

    *a = 1;
    *b = 2;

    allocator.free(marker);

    float *c = reinterpret_cast<float *>(allocator.alloc(sizeof(float)));

    *c = 3;

    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(*a, *b);
    EXPECT_NE(*a, *c);

    EXPECT_EQ(b, c) << "b and c should be pointing to the same adress";
    EXPECT_EQ(*b, *c) << "b and c should contain the same value";
}