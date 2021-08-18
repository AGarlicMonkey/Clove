#include <Clove/Memory/PoolAllocator.hpp>
#include <array>
#include <gtest/gtest.h>

using namespace clove;

TEST(PoolAllocatorTests, CanAllocateAnItem) {
    size_t constexpr numOfItems = 100;

    PoolAllocator<float> allocator(numOfItems);

    float *data = allocator.alloc();

    EXPECT_TRUE(data != nullptr);
}

TEST(PoolAllocatorTests, CanAllocateAnItemAndConstructItem) {
    int constexpr xVal   = 4;
    float constexpr bVal = 32.333f;
    struct Data {
        Data() = default;
        Data(int x, float b)
            : x(x)
            , b(b) {
        }

        int x{ 0 };
        float b{ 0.0f };
    };

    size_t constexpr numOfItems = 100;
    PoolAllocator<Data> allocator(numOfItems);

    Data *dataA = allocator.alloc();
    Data *dataB = allocator.alloc(xVal, bVal);

    EXPECT_EQ(dataA->x, 0);
    EXPECT_EQ(dataA->b, 0.0f);

    EXPECT_NE(dataB->x, 0);
    EXPECT_NE(dataB->b, 0.0f);
    EXPECT_EQ(dataB->x, xVal);
    EXPECT_EQ(dataB->b, bVal);
}

TEST(PoolAllocatorTests, CallsAnItemsDestructor) {
    struct Helper {
        Helper(bool *flag)
            : flag(flag) {
        }
        ~Helper() {
            *flag = true;
        }

        bool *flag{ nullptr };
    };

    bool wasDeconstructorCalled{ false };

    size_t constexpr numOfItems = 100;
    PoolAllocator<Helper> allocator(numOfItems);

    Helper *data = allocator.alloc(&wasDeconstructorCalled);

    EXPECT_TRUE(data->flag);

    allocator.free(data);

    EXPECT_TRUE(wasDeconstructorCalled);
}

TEST(PoolAllocatorTests, CannotAllocateMoreThanTheAllocatorHas) {
    size_t constexpr numOfItems = 5;

    PoolAllocator<float> allocator(numOfItems);

    for(size_t i = 0; i < numOfItems; ++i) {
        allocator.alloc();
    }

    float *data = allocator.alloc();

    EXPECT_FALSE(data != nullptr);
}

TEST(PoolAllocatorTests, CanFreeAndReallocateItems) {
    size_t constexpr numOfItems = 5;

    PoolAllocator<float> allocator(numOfItems);

    float *lastElem{ nullptr };
    for(size_t i = 0; i < numOfItems; ++i) {
        lastElem = allocator.alloc();
    }

    EXPECT_TRUE(lastElem != nullptr);

    allocator.free(lastElem);

    float *someData = allocator.alloc();

    EXPECT_TRUE(someData != nullptr);
}

TEST(PoolAllocatorTests, CanFreeAndReallocateAnyItem) {
    size_t constexpr numOfItems = 10;
    size_t constexpr allocIndex = 5;

    PoolAllocator<float> allocator(numOfItems);

    float *middleElem{ nullptr };
    for(size_t i = 0; i < numOfItems; ++i) {
        if(i == allocIndex) {
            middleElem = allocator.alloc();
        } else {
            auto *discarded = allocator.alloc();
        }
    }

    EXPECT_TRUE(middleElem != nullptr);

    allocator.free(middleElem);

    float *someData = allocator.alloc();

    EXPECT_TRUE(someData != nullptr);
}

TEST(PoolAllocatorTests, AllocatorCanGrow) {
    size_t constexpr numOfItems = 5;

    PoolAllocator<float, AllocatorStrategy::Dynamic> allocator(numOfItems);

    for(size_t i = 0; i < numOfItems; ++i){
        auto *elem = allocator.alloc();
    }

    float constexpr value = 32.0f;
    auto *elem            = allocator.alloc(value);

    EXPECT_TRUE(elem != nullptr);
    EXPECT_EQ(value, *elem);
}