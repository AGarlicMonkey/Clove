#include <Clove/Reflection/Reflection.hpp>
#include <gtest/gtest.h>

using namespace clove;

struct TestAttribute {
    std::string text{};
};

class PublicReflectClass {
public:
    int x;
    float y;
};

CLOVE_REFLECT_DECLARE_TYPE(PublicReflectClass)

CLOVE_REFLECT_BEGIN(PublicReflectClass, TestAttribute{ .text = "class" })
CLOVE_REFLECT_MEMBER(x, TestAttribute{ .text = "member" })
CLOVE_REFLECT_MEMBER(y)
CLOVE_REFLECT_END

class PrivateReflectClass {
    CLOVE_REFLECT_PRIVATE(PrivateReflectClass)

private:
    int a;
    int b;

public:
    //Helper functions for tests
    size_t offsetOfA() const {
        return offsetof(PrivateReflectClass, a);
    };
    size_t sizeOfA() const {
        return sizeof(a);
    };

    size_t offsetOfB() const {
        return offsetof(PrivateReflectClass, b);
    };
    size_t sizeOfB() const {
        return sizeof(b);
    };
};

CLOVE_REFLECT_DECLARE_TYPE(PrivateReflectClass)

CLOVE_REFLECT_BEGIN(PrivateReflectClass)
CLOVE_REFLECT_MEMBER(a)
CLOVE_REFLECT_MEMBER(b)
CLOVE_REFLECT_END

class NotReflectedClass {
public:
    int x;
    float y;
};

namespace test {
    class NamespaceReflectedClass {
    public:
        int a;
        int b;
    };
}

CLOVE_REFLECT_DECLARE_TYPE(test::NamespaceReflectedClass)

CLOVE_REFLECT_BEGIN(test::NamespaceReflectedClass)
CLOVE_REFLECT_MEMBER(a)
CLOVE_REFLECT_MEMBER(b)
CLOVE_REFLECT_END

struct Internal {
    int a;
    std::string b;
};

struct Nested {
    Internal a;
    int b;
};

CLOVE_REFLECT_DECLARE_TYPE(Internal)

CLOVE_REFLECT_BEGIN(Internal)
CLOVE_REFLECT_MEMBER(a)
CLOVE_REFLECT_MEMBER(b)
CLOVE_REFLECT_END

CLOVE_REFLECT_DECLARE_TYPE(Nested)

CLOVE_REFLECT_BEGIN(Nested)
CLOVE_REFLECT_MEMBER(a)
CLOVE_REFLECT_MEMBER(b)
CLOVE_REFLECT_END

TEST(ReflectionTests, CanGetTypeInfoByType) {
    reflection::TypeInfo const *publicClassInfo{ reflection::getTypeInfo<PublicReflectClass>() };
    reflection::TypeInfo const *privateClassInfo{ reflection::getTypeInfo<PrivateReflectClass>() };
    reflection::TypeInfo const *notReflectedClassInfo{ reflection::getTypeInfo<NotReflectedClass>() };

    EXPECT_NE(publicClassInfo, nullptr);
    EXPECT_NE(privateClassInfo, nullptr);
    EXPECT_EQ(notReflectedClassInfo, nullptr);
}

TEST(ReflectionTests, CanGetTypeInfoByName) {
    reflection::TypeInfo const *publicClassInfo{ reflection::getTypeInfo("PublicReflectClass") };
    reflection::TypeInfo const *privateClassInfo{ reflection::getTypeInfo("PrivateReflectClass") };
    reflection::TypeInfo const *notReflectedClassInfo{ reflection::getTypeInfo("NotReflectedClass") };

    EXPECT_NE(publicClassInfo, nullptr);
    EXPECT_NE(privateClassInfo, nullptr);
    EXPECT_EQ(notReflectedClassInfo, nullptr);
}

TEST(ReflectionTests, CanGetTypeInfoById) {
    reflection::TypeInfo const *publicClassInfo{ reflection::getTypeInfo(typeid(PublicReflectClass).hash_code()) };
    reflection::TypeInfo const *privateClassInfo{ reflection::getTypeInfo(typeid(PrivateReflectClass).hash_code()) };
    reflection::TypeInfo const *notReflectedClassInfo{ reflection::getTypeInfo(typeid(NotReflectedClass).hash_code()) };

    EXPECT_NE(publicClassInfo, nullptr);
    EXPECT_NE(privateClassInfo, nullptr);
    EXPECT_EQ(notReflectedClassInfo, nullptr);
}

TEST(ReflectionTests, TypeInfoIdIsCorrect) {
    reflection::TypeInfo const *typeInfo{ reflection::getTypeInfo<PublicReflectClass>() };

    EXPECT_EQ(typeInfo->id, typeid(PublicReflectClass).hash_code());
}

TEST(ReflectionTests, TypeInfoSizeIsCorrect) {
    reflection::TypeInfo const *typeInfo{ reflection::getTypeInfo<PublicReflectClass>() };

    EXPECT_EQ(typeInfo->size, sizeof(PublicReflectClass));
}

TEST(ReflectionTests, CanGetTypeAttributes) {
    reflection::TypeInfo const *publicClassInfo{ reflection::getTypeInfo<PublicReflectClass>() };
    reflection::TypeInfo const *privateClassInfo{ reflection::getTypeInfo<PrivateReflectClass>() };

    std::optional<TestAttribute> publicClassAttribute{ publicClassInfo->attributes.get<TestAttribute>() };
    std::optional<TestAttribute> privateClassAttribute{ privateClassInfo->attributes.get<TestAttribute>() };

    EXPECT_TRUE(publicClassAttribute.has_value());
    EXPECT_FALSE(privateClassAttribute.has_value());
}

TEST(ReflectionTests, CanGetValueOfTypeAttributes) {
    reflection::TypeInfo const *publicClassInfo{ reflection::getTypeInfo<PublicReflectClass>() };
    std::optional<TestAttribute> publicClassAttribute{ publicClassInfo->attributes.get<TestAttribute>() };

    ASSERT_TRUE(publicClassAttribute.has_value());
    EXPECT_EQ(publicClassAttribute->text, "class");
}

TEST(ReflectionTests, CanGetAllTypesWithAttribute) {
    std::vector<reflection::TypeInfo const *> typeInfos{ reflection::getTypesWithAttribute<TestAttribute>() };

    ASSERT_EQ(typeInfos.size(), 1);
    EXPECT_EQ(typeInfos[0]->name, "PublicReflectClass");
}

TEST(ReflectionTests, CanGetNumTypePublicMembers) {
    size_t constexpr memberCount{ 2 };
    reflection::TypeInfo const *publicClassInfo{ reflection::getTypeInfo<PublicReflectClass>() };

    EXPECT_EQ(publicClassInfo->members.size(), memberCount);
}

TEST(ReflectionTests, CanGetNumTypePrivateMembers) {
    size_t constexpr memberCount{ 2 };
    reflection::TypeInfo const *privateClassInfo{ reflection::getTypeInfo<PrivateReflectClass>() };

    EXPECT_EQ(privateClassInfo->members.size(), memberCount);
}

TEST(ReflectionTests, CanGetBasicMemberInfo) {
    std::vector<reflection::MemberInfo> const &publicClassMembers{ reflection::getTypeInfo<PublicReflectClass>()->members };

    EXPECT_EQ(publicClassMembers[0].name, "x");
    EXPECT_EQ(publicClassMembers[0].offset, offsetof(PublicReflectClass, x));
    EXPECT_EQ(publicClassMembers[0].size, sizeof(PublicReflectClass::x));

    EXPECT_EQ(publicClassMembers[1].name, "y");
    EXPECT_EQ(publicClassMembers[1].offset, offsetof(PublicReflectClass, y));
    EXPECT_EQ(publicClassMembers[1].size, sizeof(PublicReflectClass::y));

    PublicReflectClass pubClassInstance{};
    pubClassInstance.x = 100;
    pubClassInstance.y = 32.5f;

    auto *const pubClassPtr{ reinterpret_cast<std::byte *>(&pubClassInstance) };
    auto *const x{ reinterpret_cast<int *>(pubClassPtr + publicClassMembers[0].offset) };
    auto *const y{ reinterpret_cast<float *>(pubClassPtr + publicClassMembers[1].offset) };

    EXPECT_EQ(*x, pubClassInstance.x);
    EXPECT_EQ(*y, pubClassInstance.y);

    std::vector<reflection::MemberInfo> const &privateClassMembers{ reflection::getTypeInfo<PrivateReflectClass>()->members };
    PrivateReflectClass sizeHelper{};

    EXPECT_EQ(privateClassMembers[0].name, "a");
    EXPECT_EQ(privateClassMembers[0].offset, sizeHelper.offsetOfA());
    EXPECT_EQ(privateClassMembers[0].size, sizeHelper.sizeOfA());

    EXPECT_EQ(privateClassMembers[1].name, "b");
    EXPECT_EQ(privateClassMembers[1].offset, sizeHelper.offsetOfB());
    EXPECT_EQ(privateClassMembers[1].size, sizeHelper.sizeOfB());

    auto const *internalInfo{ reflection::getTypeInfo<Internal>() };
    auto const &internalInfoMembers{ internalInfo->members };

    Internal internalInstance{};
    internalInstance.b = "Hello, World!";

    auto *const internalClassPtr{ reinterpret_cast<std::byte *>(&internalInstance) };
    auto *const b{ reinterpret_cast<std::string *>(internalClassPtr + internalInfoMembers[1].offset) };

    EXPECT_EQ(*b, internalInstance.b);
}

TEST(ReflectionTests, CanGetMemberAttributes) {
    auto const &members{ reflection::getTypeInfo<PublicReflectClass>()->members };

    EXPECT_TRUE(members[0].attributes.contains<TestAttribute>());
    EXPECT_FALSE(members[1].attributes.contains<TestAttribute>());
}

TEST(ReflectionTests, CanGetValueOfMemberAttributes) {
    auto const &members{ reflection::getTypeInfo<PublicReflectClass>()->members };
    std::optional<TestAttribute> attribute{ members[0].attributes.get<TestAttribute>() };

    ASSERT_TRUE(attribute.has_value());
    EXPECT_EQ(attribute->text, "member");
}

TEST(ReflectionTests, MembersHaveTypeInfoIds) {
    auto const *nestedInfo{ reflection::getTypeInfo<Nested>() };
    auto const *internalInfo{ reflection::getTypeInfo<Internal>() };

    EXPECT_EQ(nestedInfo->members[0].id, internalInfo->id);

    auto *internalTypeInfo{ reflection::getTypeInfo(nestedInfo->members[0].id) };

    ASSERT_NE(internalTypeInfo, nullptr);
    EXPECT_EQ(internalTypeInfo->id, internalInfo->id);
}