#include <Clove/Delegate/MultiCastDelegate.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(MultiCastDelegateTests, CanBroadcastADelegate) {
    bool called{ false };

    MultiCastDelegate<void()> delegate;

    auto handle = delegate.bind([&called]() {
        called = true;
    });

    delegate.broadcast();

    EXPECT_TRUE(called);
}

TEST(MultiCastDelegateTests, CanBroadcastManyDelegates) {
    bool calledOne{ false };
    bool calledTwo{ false };

    MultiCastDelegate<void()> delegate;

    auto handleOne = delegate.bind([&calledOne]() {
        calledOne = true;
    });
    auto handleTwo = delegate.bind([&calledTwo]() {
        calledTwo = true;
    });

    delegate.broadcast();

    EXPECT_TRUE(calledOne);
    EXPECT_TRUE(calledTwo);
}

TEST(MultiCastDelegateTests, CanBindAMemberFunction) {
    class Object {
    public:
        bool called{ false };

    public:
        void setCalledToTrue() {
            called = true;
        }
    };

    Object myObj;

    MultiCastDelegate<void()> delegate;

    auto handle = delegate.bind(&Object::setCalledToTrue, &myObj);

    delegate.broadcast();

    EXPECT_TRUE(myObj.called);
}

TEST(MultiCastDelegateTests, CanUnbind) {
    bool called{ false };

    MultiCastDelegate<void()> delegate;

    DelegateHandle handle;

    EXPECT_FALSE(handle.isValid());

    handle = delegate.bind([&called]() {
        called = true;
    });

    EXPECT_TRUE(handle.isValid());

    delegate.unbind(handle);

    delegate.broadcast();

    EXPECT_FALSE(handle.isValid());
    EXPECT_FALSE(called);
}

TEST(MultiCastDelegateTests, HandleAutomaticallyUnbinds) {
    bool called{ false };

    MultiCastDelegate<void()> delegate;

    {
        auto handle = delegate.bind([&called]() {
            called = true;
        });
    }

    EXPECT_FALSE(called);
}

TEST(MultiCastDelegateTests, DelegateUnbindsWhenDestroyed) {
    bool called{ false };

    DelegateHandle handle;

    {
        MultiCastDelegate<void()> delegate;

        handle = delegate.bind([&called]() {
            called = true;
        });
    }

    EXPECT_FALSE(handle.isValid());
    EXPECT_FALSE(called);
}

TEST(MultiCastDelegateTests, CanUseParamsInBroadcast) {
    int32_t constexpr aVal = 42;
    float constexpr yVal   = 1046.1123f;
    std::vector<int32_t> vector{ 1, 2, 3, 4 };
    double constexpr dVal = 10383758923.123123;

    MultiCastDelegate<void(int32_t)> oneParamDelegate;
    MultiCastDelegate<void(int32_t, float)> twoParamDelegate;
    MultiCastDelegate<void(int32_t, std::vector<int32_t>, double)> threeParamDelegate;

    auto handleOne = oneParamDelegate.bind([&](int32_t a) {
        EXPECT_EQ(aVal, a);
    });
    oneParamDelegate.broadcast(aVal);

    auto handleTwo = twoParamDelegate.bind([&](int32_t a, float y) {
        EXPECT_EQ(aVal, a);
        EXPECT_EQ(yVal, y);
    });
    twoParamDelegate.broadcast(aVal, yVal);

    auto handleThree = threeParamDelegate.bind([&](int32_t a, std::vector<int32_t> v, double d) {
        EXPECT_EQ(aVal, a);
        EXPECT_EQ(std::size(vector), std::size(v));
        EXPECT_EQ(vector[0], v[0]);
        EXPECT_EQ(vector[1], v[1]);
        EXPECT_EQ(vector[2], v[2]);
        EXPECT_EQ(vector[3], v[3]);
        EXPECT_EQ(dVal, d);
    });
    threeParamDelegate.broadcast(aVal, vector, dVal);
}