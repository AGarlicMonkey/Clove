#include <Clove/Memory/ListAllocator.hpp>
#include <array>
#include <gtest/gtest.h>

using namespace clove;

static size_t constexpr allocatorSize{ 1024 };

TEST(ListAllocatorTests, CanAllocateAnAmountOfBytes) {
    ListAllocator allocator{ allocatorSize };

    void *data1{ allocator.alloc(100, 0) };
    uint32_t *data2{ allocator.alloc<uint32_t>() };

    EXPECT_TRUE(data1 != nullptr);
    EXPECT_TRUE(data2 != nullptr);
}

TEST(ListAllocatorTests, AllocationsAreProperlyAligned) {
    ListAllocator allocator{ allocatorSize };

    size_t constexpr alignment{ 16 };

    void *data1{ allocator.alloc(100, alignment) };
    uint32_t *data2{ allocator.alloc<uint32_t>() };

    EXPECT_EQ(reinterpret_cast<uintptr_t>(data1) % alignment, 0);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(data2) % alignof(uint32_t), 0);
}

TEST(ListAllocatorTests, MultipleAllocationsDoNotOverlap) {
    ListAllocator allocator{ allocatorSize };

    size_t constexpr alignment{ 16 };

    auto *data1{ allocator.alloc<uint32_t>() };
    auto *data2{ allocator.alloc<uint64_t>() };
    auto *data3{ allocator.alloc<float>() };
    auto *data4{ allocator.alloc<uint8_t>() };
    auto *data5{ allocator.alloc<uint32_t>() };
    auto *data6{ allocator.alloc<uint32_t>() };

    uint32_t constexpr data1Value{ 100000000 };
    uint64_t constexpr data2Value{ 12345678987654321 };
    float constexpr data3Value{ 3.13754943 };
    uint8_t constexpr data4Value{ 19 };
    uint32_t constexpr data5Value{ 10001 };
    uint32_t constexpr data6Value{ 1002 };

    *data1 = data1Value;
    *data2 = data2Value;
    *data3 = data3Value;
    *data4 = data4Value;
    *data5 = data5Value;
    *data6 = data6Value;

    EXPECT_EQ(*data1, data1Value);
    EXPECT_EQ(*data2, data2Value);
    EXPECT_EQ(*data3, data3Value);
    EXPECT_EQ(*data4, data4Value);
    EXPECT_EQ(*data5, data5Value);
    EXPECT_EQ(*data6, data6Value);
}

TEST(ListAllocatorTests, CannotAllocateMoreThanTheAllocatorHas) {
    ListAllocator allocator{ allocatorSize };

    void *data{ allocator.alloc(allocatorSize + 1, 0) };

    EXPECT_FALSE(data != nullptr);
}

TEST(ListAllocatorTests, CanFreeAndReallocateBytes) {
    ListAllocator allocator{ allocatorSize };

    void *allData{ allocator.alloc(allocatorSize, 0) };

    EXPECT_TRUE(allData != nullptr);

    allocator.free(allData);

    void *someData{ allocator.alloc(500, 0) };

    EXPECT_TRUE(someData != nullptr);
}

TEST(ListAllocatorTests, CanInitialiseWithARangeOfMemory) {
    std::array<std::byte, allocatorSize> allocatorRange;
    ListAllocator allocator{ allocatorRange.data(), allocatorRange.size() };

    void *data{ allocator.alloc(100, 0) };

    EXPECT_TRUE(data != nullptr);
}

TEST(ListAllocatorTests, FreeWillFreeAndReallocTheCorrectRange) {
    ListAllocator allocator{ allocatorSize };

    uint32_t constexpr value{ 5 };

    uint32_t *first{ allocator.alloc<uint32_t>() };
    *first = value;

    allocator.free(first);

    uint32_t *second{ allocator.alloc<uint32_t>() };

    EXPECT_EQ(value, *second);
}

TEST(ListAllocatorTests, AllocatedBlocksAreClampedtoCorrectSize) {
    ListAllocator allocator{ allocatorSize };

    void *all{ allocator.alloc(allocatorSize, 0) };
    allocator.free(all);

    uint32_t *first{ allocator.alloc<uint32_t>() };
    uint32_t *second{ allocator.alloc<uint32_t>() };

    EXPECT_TRUE(first != nullptr);
    EXPECT_TRUE(second != nullptr);
}

TEST(ListAllocatorTests, AllocatedBlocksAreMergedAfterBeingFreed) {
    ListAllocator allocator{ allocatorSize };

    std::vector<void *> allocations{ allocatorSize, nullptr };
    for(size_t i{ 0 }; i < allocatorSize; ++i) {
        if(void *allocation{ allocator.alloc(1, 0) }) {
            allocations[i] = allocation;
        } else {
            break;
        }
    }
    for(void *allocation : allocations) {
        allocator.free(allocation);
    }

    void *data{ allocator.alloc(1000, 0) };

    EXPECT_TRUE(data != nullptr);

    allocations.resize(0);
}

TEST(ListAllocatorTests, CanGetSizeOfAllocator) {
    ListAllocator allocator{ allocatorSize };

    EXPECT_EQ(allocator.getSize(), allocatorSize);
}

TEST(ListAllocatorTests, CanGetMemoryOfAllocator) {
    ListAllocator allocator{ allocatorSize };

    EXPECT_NE(allocator.getMemory(), nullptr);
}