#include <Clove/Event/EventDispatcher.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(EventTests, CanBroadcastAnEvent) {
    struct Event {
        int32_t x{ 0 };
    };
    int32_t constexpr xVal{ 32 };
    bool called{ false };

    EventDispatcher dispatcher{};

    auto handle = dispatcher.bindToEvent<Event>([xVal, &called](Event const& event) {
        EXPECT_EQ(xVal, event.x);
        called = true;
    });

    Event testEvent{
        .x = xVal,
    };

    dispatcher.broadCastEvent(testEvent);

    EXPECT_TRUE(called);
}

TEST(EventTests, CanBroadcastDifferentTypesOfEvents) {
    struct EventTypeA {
        int32_t x{ 0 };
    };
    struct EventTypeB {
        float x{ 0 };
        float y{ 0 };
        float z{ 0 };
    };

    bool aCalled{ false };
    bool bCalled{ false };

    EventDispatcher dispatcher{};

    auto aHandle = dispatcher.bindToEvent<EventTypeA>([&aCalled](EventTypeA const& event) {
        aCalled = true;
    });
    auto bHandle = dispatcher.bindToEvent<EventTypeB>([&bCalled](EventTypeB const& event) {
        bCalled = true;
    });

    EventTypeA testEventA{};
    EventTypeB testEventB{};

    dispatcher.broadCastEvent(testEventA);

    EXPECT_TRUE(aCalled);
    EXPECT_FALSE(bCalled);

    dispatcher.broadCastEvent(testEventB);

    EXPECT_TRUE(bCalled);
}

TEST(EventTests, CanUnbindFromEvents) {
    struct Event {
        int32_t x{ 0 };
    };
    bool called{ false };

    EventDispatcher dispatcher{};

    auto handle = dispatcher.bindToEvent<Event>([&called](Event const& event) {
        called = true;
    });

    dispatcher.unbindFromEvent<Event>(handle);

    Event testEvent{};

    dispatcher.broadCastEvent(testEvent);

    EXPECT_FALSE(called);
}

TEST(EventTests, HandleAutomaticallyUnbindsFromEvents) {
    struct Event {
        int32_t x{ 0 };
    };
    bool called{ false };

    EventDispatcher dispatcher{};

    {
        auto handle = dispatcher.bindToEvent<Event>([&called](Event const& event) {
            called = true;
        });
    }

    Event testEvent{};

    dispatcher.broadCastEvent(testEvent);

    EXPECT_FALSE(called);
}

TEST(EventTests, HandleCanManuallyUnbindFromEvents) {
    struct Event {
        int32_t x{ 0 };
    };
    bool called{ false };

    EventDispatcher dispatcher{};

    auto handle = dispatcher.bindToEvent<Event>([&called](Event const& event) {
        called = true;
    });

    handle.reset();

    Event testEvent{};

    dispatcher.broadCastEvent(testEvent);

    EXPECT_FALSE(called);
}

TEST(EventTests, DispatcherResetsHandles) {
    struct Event {
        int32_t x{ 0 };
    };
    bool called{ false };

    EventHandle handle;

    {
        EventDispatcher dispatcher{};

        handle = dispatcher.bindToEvent<Event>([&called](Event const& event) {
            called = true;
        });

        EXPECT_TRUE(handle.isValid());
    }

    EXPECT_FALSE(handle.isValid());
}