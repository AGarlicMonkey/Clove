#include <Clove/Delegate/SingleCastDelegate.hpp>
#include <gtest/gtest.h>
#include <vector>

using namespace clove;

TEST(SingleCastDelegateTests, CanBroadcastADelegate) {
    bool called{ false };

    SingleCastDelegate<void()> delegate;

    delegate.bind([&called]() {
        called = true;
    });

    delegate.broadcast();

    EXPECT_TRUE(called);
}

TEST(SingleCastDelegateTests, CanBindAMemberFunction) {
    class Object {
    public:
        bool called{ false };

    public:
        void setCalledToTrue() {
            called = true;
        }
    };

    Object myObj;

    SingleCastDelegate<void()> delegate;

    delegate.bind(&Object::setCalledToTrue, &myObj);

    delegate.broadcast();

    EXPECT_TRUE(myObj.called);
}

TEST(SingleCastDelegateTests, CanUnbind) {
    SingleCastDelegate<void()> delegate;

    EXPECT_FALSE(delegate.isBound());
    EXPECT_FALSE(delegate);

    delegate.bind([]() {
        //Empty
    });

    EXPECT_TRUE(delegate.isBound());
    EXPECT_TRUE(delegate);

    delegate.unbind();

    EXPECT_FALSE(delegate.isBound());
    EXPECT_FALSE(delegate);
}

TEST(SingleCastDelegateTests, CanUseParamsInBroadcast) {
    int32_t constexpr aVal = 42;
    float constexpr yVal   = 1046.1123f;
    std::vector<int32_t> vector{ 1, 2, 3, 4 };
    double constexpr dVal = 10383758923.123123;

    SingleCastDelegate<void(int32_t)> oneParamDelegate;
    SingleCastDelegate<void(int32_t, float)> twoParamDelegate;
    SingleCastDelegate<void(int32_t, std::vector<int32_t>, double)> threeParamDelegate;

    oneParamDelegate.bind([&](int32_t a) {
        EXPECT_EQ(aVal, a);
    });
    oneParamDelegate.broadcast(aVal);

    twoParamDelegate.bind([&](int32_t a, float y) {
        EXPECT_EQ(aVal, a);
        EXPECT_EQ(yVal, y);
    });
    twoParamDelegate.broadcast(aVal, yVal);

    threeParamDelegate.bind([&](int32_t a, std::vector<int32_t> v, double d) {
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