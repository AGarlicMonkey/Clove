#include <Clove/ECS/EntityManager.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(ECSEntityTests, DefaultInitialiseEntity) {
    Entity entity{};

    EXPECT_EQ(entity, NullEntity);
}

TEST(ECSEntityTests, CreateEntity) {
    EntityManager manager{};
    Entity entity{ manager.create() };

    EXPECT_NE(entity, NullEntity);

    std::vector<Entity> entities{};
    for(int i{ 0 }; i < 10000; ++i) {
        Entity entity{ manager.create() };

        entities.push_back(entity);

        for(int j{ i - 1 }; j >= 0; --j) {
            EXPECT_NE(entity, entities[j]);
        }
    }
}

TEST(ECSEntityTests, CheckEntityValidity) {
    EntityManager manager{};

    EXPECT_FALSE(manager.isValid(NullEntity));

    Entity entity{ manager.create() };
    ASSERT_TRUE(manager.isValid(entity));

    Entity clone{ manager.clone(entity) };
    EXPECT_TRUE(manager.isValid(clone));

    Entity badEntity{ 99999999 };
    EXPECT_FALSE(manager.isValid(badEntity));

    EntityManager otherManager{};

    Entity wrongEntity{ otherManager.create() };

    EXPECT_FALSE(manager.isValid(wrongEntity));
    EXPECT_EQ(manager.clone(wrongEntity), NullEntity);
}

TEST(ECSEntityTests, DestroySingleEntity){
    EntityManager manager{};
    Entity entity{ manager.create() };
    manager.addComponent<uint32_t>(entity);

    ASSERT_TRUE(manager.isValid(entity));
    ASSERT_TRUE(manager.hasComponent<uint32_t>(entity));

    manager.destroy(entity);

    EXPECT_FALSE(manager.isValid(entity));
    EXPECT_FALSE(manager.hasComponent<uint32_t>(entity));

    ASSERT_TRUE(CLOVE_ENABLE_ASSERTIONS);

    EXPECT_DEATH(manager.getComponent<uint32_t>(entity), "");
    EXPECT_DEATH(manager.addComponent<float>(entity), "");
}

TEST(ECSEntityTests, DestroyAllEntities) {
    EntityManager manager{};
    Entity entityA{ manager.create() };
    Entity entityB{ manager.create() };
    Entity entityC{ manager.create() };
    Entity entityD{ manager.create() };

    ASSERT_TRUE(manager.isValid(entityA));
    ASSERT_TRUE(manager.isValid(entityB));
    ASSERT_TRUE(manager.isValid(entityC));
    ASSERT_TRUE(manager.isValid(entityD));

    manager.destroyAll();

    EXPECT_FALSE(manager.isValid(entityA));
    EXPECT_FALSE(manager.isValid(entityB));
    EXPECT_FALSE(manager.isValid(entityC));
    EXPECT_FALSE(manager.isValid(entityD));
}