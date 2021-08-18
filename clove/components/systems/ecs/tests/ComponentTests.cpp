#include <Clove/ECS/EntityManager.hpp>
#include <gtest/gtest.h>

using namespace clove;

struct BoolComponent {
    bool value{ false };
};

struct ComplexComponent {
    int32_t a{ 0 };
    int32_t b{ 0 };
    int32_t c{ 0 };
};

struct FloatComponent {
    float value{ 0.0f };
};

TEST(ECSComponentTests, CanAddSingleComponentToEntity) {
    EntityManager manager{};

    Entity defaultEntity{ manager.create() };
    Entity falseEntity{ manager.create() };
    Entity trueEntity{ manager.create() };

    auto &defaultComp{ manager.addComponent<BoolComponent>(defaultEntity) };
    EXPECT_EQ(defaultComp.value, BoolComponent{}.value);

    auto &falseComp{ manager.addComponent<BoolComponent>(falseEntity, false) };
    EXPECT_FALSE(falseComp.value);

    auto &trueComp{ manager.addComponent<BoolComponent>(trueEntity, true) };
    EXPECT_TRUE(trueComp.value);

    Entity complexEntity{ manager.create() };

    auto &complexComp{ manager.addComponent<ComplexComponent>(complexEntity, 1, 2, 3) };
    EXPECT_EQ(complexComp.a, 1);
    EXPECT_EQ(complexComp.b, 2);
    EXPECT_EQ(complexComp.c, 3);

    Entity trueOrFalseEntity{ manager.create() };

    auto &trueOrFalseCompA{ manager.addComponent<BoolComponent>(trueOrFalseEntity, false) };
    EXPECT_FALSE(trueOrFalseCompA.value);

    auto &trueOrFalseCompB{ manager.addComponent<BoolComponent>(trueOrFalseEntity, true) };
    EXPECT_TRUE(trueOrFalseCompA.value);
    EXPECT_TRUE(trueOrFalseCompB.value);

    trueOrFalseCompA.value = false;
    EXPECT_FALSE(trueOrFalseCompA.value);
    EXPECT_FALSE(trueOrFalseCompB.value);

    ASSERT_TRUE(CLOVE_ENABLE_ASSERTIONS);

    EXPECT_DEATH(manager.addComponent<BoolComponent>(NullEntity), "");
    
    Entity badEntity{ 999999999 };
    EXPECT_DEATH(manager.addComponent<BoolComponent>(badEntity), "");
}

TEST(ECSComponentTests, CanCheckIfAnEntityHasAComponent) {
    EntityManager manager{};

    Entity entity{ manager.create() };

    manager.addComponent<BoolComponent>(entity);
    EXPECT_TRUE(manager.hasComponent<BoolComponent>(entity));
    EXPECT_FALSE(manager.hasComponent<FloatComponent>(entity));

    EXPECT_FALSE(manager.hasComponent<FloatComponent>(NullEntity));

    Entity badEntity{ 999999999 };
    EXPECT_FALSE(manager.hasComponent<BoolComponent>(badEntity));
}

TEST(ECSComponentTests, CanGetAnEntitysComponent) {
    ASSERT_TRUE(CLOVE_ENABLE_ASSERTIONS);

    EntityManager manager{};

    Entity entity{ manager.create() };
    manager.addComponent<BoolComponent>(entity, true);

    auto trueComp{ manager.getComponent<BoolComponent>(entity) };
    EXPECT_TRUE(trueComp.value);
    EXPECT_DEATH(manager.getComponent<FloatComponent>(entity), "");

    EXPECT_DEATH(manager.getComponent<FloatComponent>(NullEntity), "");

    Entity badEntity{ 999999999 };
    EXPECT_DEATH(manager.getComponent<FloatComponent>(badEntity), "");
}

TEST(ECSComponentTests, CanAddMultipleComponentsToEntity) {
    EntityManager manager{};

    Entity entity{ manager.create() };
    manager.addComponent<BoolComponent>(entity);
    manager.addComponent<FloatComponent>(entity);

    EXPECT_TRUE(manager.hasComponent<BoolComponent>(entity));
    EXPECT_TRUE(manager.hasComponent<FloatComponent>(entity));
}

TEST(ECSComponentTests, CanRemoveComponentFromEntity) {
    EntityManager manager{};

    Entity entity{ manager.create() };
    manager.addComponent<BoolComponent>(entity);
    manager.addComponent<FloatComponent>(entity);

    EXPECT_TRUE(manager.hasComponent<BoolComponent>(entity));
    EXPECT_TRUE(manager.hasComponent<FloatComponent>(entity));

    manager.removeComponent<FloatComponent>(entity);

    EXPECT_FALSE(manager.hasComponent<FloatComponent>(entity));

    ASSERT_TRUE(CLOVE_ENABLE_ASSERTIONS);

    EXPECT_DEATH(manager.getComponent<FloatComponent>(entity), "");

    Entity entityA{ manager.create() };
    Entity entityB{ manager.create() };

    manager.addComponent<BoolComponent>(entityA, false);
    manager.addComponent<BoolComponent>(entityB, true);

    EXPECT_FALSE(manager.getComponent<BoolComponent>(entityA).value);
    EXPECT_TRUE(manager.getComponent<BoolComponent>(entityB).value);

    manager.removeComponent<BoolComponent>(entityA);

    EXPECT_TRUE(manager.hasComponent<BoolComponent>(entityB));
    EXPECT_TRUE(manager.getComponent<BoolComponent>(entityB).value);
}

TEST(ECSComponentTests, CanCloneEntities) {
    EntityManager manager{};

    Entity entityA{ manager.create() };

    manager.addComponent<ComplexComponent>(entityA, 1, 2, 3);

    Entity entityB{ manager.clone(entityA) };

    auto aComp{ manager.getComponent<ComplexComponent>(entityA) };
    auto bComp{ manager.getComponent<ComplexComponent>(entityB) };

    EXPECT_EQ(aComp.a, bComp.a);
    EXPECT_EQ(aComp.b, bComp.b);
    EXPECT_EQ(aComp.c, bComp.c);

    aComp.a = 100;
    aComp.b = 200;
    aComp.c = 300;

    EXPECT_NE(aComp.a, bComp.a);
    EXPECT_NE(aComp.b, bComp.b);
    EXPECT_NE(aComp.c, bComp.c);
}