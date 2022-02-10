#include <Clove/Blackboard.hpp>

#include <gtest/gtest.h>

using namespace clove;

TEST(BlackboardTests, CanCopyBlackboard) {
    struct ComplexType{
        int i{};
        std::string s{};
    };

    Blackboard a{};
    Blackboard b{};

    Blackboard::Key constexpr intKey{ 0 };
    Blackboard::Key constexpr floatKey{ 1 };
    Blackboard::Key constexpr complexKey{ 2 };

    int constexpr intVal{ 32 };
    float constexpr floatVal{ 32.5 };
    ComplexType const complexVal{ 100, "Hello, World! This is a long string to make sure it's all good" };

    a.setValue(intKey, intVal);
    a.setValue(floatKey, floatVal);
    a.setValue(complexKey, complexVal);

    b = a;
    Blackboard c{ a };

    ASSERT_TRUE(b.hasValue(intKey));
    ASSERT_TRUE(b.hasValue(floatKey));
    ASSERT_TRUE(b.hasValue(complexKey));

    ASSERT_TRUE(c.hasValue(intKey));
    ASSERT_TRUE(c.hasValue(floatKey));
    ASSERT_TRUE(b.hasValue(complexKey));

    EXPECT_EQ(a.getValue<int>(intKey).value(), b.getValue<int>(intKey).value());
    EXPECT_EQ(a.getValue<int>(intKey).value(), c.getValue<int>(intKey).value());

    EXPECT_EQ(a.getValue<float>(floatKey).value(), b.getValue<float>(floatKey).value());
    EXPECT_EQ(a.getValue<float>(floatKey).value(), c.getValue<float>(floatKey).value());

    EXPECT_EQ(a.getValue<ComplexType>(complexKey).value().i, b.getValue<ComplexType>(complexKey).value().i);
    EXPECT_EQ(a.getValue<ComplexType>(complexKey).value().i, c.getValue<ComplexType>(complexKey).value().i);
    EXPECT_EQ(a.getValue<ComplexType>(complexKey).value().s, b.getValue<ComplexType>(complexKey).value().s);
    EXPECT_EQ(a.getValue<ComplexType>(complexKey).value().s, c.getValue<ComplexType>(complexKey).value().s);

    b.setValue(intKey, 1);
    c.setValue(intKey, 2);
    b.setValue(complexKey, ComplexType{ 1, "Another long string to make sure it's all good" });

    EXPECT_NE(a.getValue<int>(intKey).value(), b.getValue<int>(intKey).value());
    EXPECT_NE(a.getValue<int>(intKey).value(), c.getValue<int>(intKey).value());
    EXPECT_NE(a.getValue<ComplexType>(complexKey).value().i, b.getValue<ComplexType>(complexKey).value().i);
    EXPECT_NE(a.getValue<ComplexType>(complexKey).value().s, b.getValue<ComplexType>(complexKey).value().s);
}