#include <Clove/Expected.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(ExpectedTests, CanConstructWithAValue) {
    int32_t constexpr testValue = 50;
    Expected<int32_t, std::exception> expectedValue(testValue);

    EXPECT_EQ(testValue, expectedValue.getValue());
}

TEST(ExpectedTests, CanConstructWithAnError) {
    using ErrorType           = int32_t;
    ErrorType constexpr error = 42;

    Expected<int32_t, ErrorType> expectedValue(Unexpected<ErrorType>{ error });

    EXPECT_EQ(error, expectedValue.getError());
}

TEST(ExpectedTests, CanImplicitlyConstructUnexpected) {
    int32_t constexpr testValue{ 5 };
    struct ErrorType {
        int x{ testValue };
    };

    Expected<float, ErrorType> expected(ErrorType{});

    EXPECT_EQ(testValue, expected.getError().x);
}

TEST(ExpectedTests, CallsDestructorOnValueType) {
    struct Helper {
    public:
        bool &flag;

    public:
        Helper(bool &flag)
            : flag(flag) {
        }
        ~Helper() {
            flag = true;
        }
    };

    bool destructorCalled{ false };

    {
        Expected<Helper, std::exception> expected{ Helper{ destructorCalled } };
    }

    EXPECT_TRUE(destructorCalled);
}

TEST(ExpectedTests, CallsDestructorOnErrorType) {
    struct Helper {
    public:
        bool &flag;

    public:
        Helper(bool &flag)
            : flag(flag) {
        }
        ~Helper() {
            flag = true;
        }
    };

    bool destructorCalled{ false };

    {
        Expected<int32_t, Helper> expected{ Unexpected<Helper>{ Helper{ destructorCalled } } };
    }

    EXPECT_TRUE(destructorCalled);
}

TEST(ExpectedTests, CanCheckIfThereIsAnError) {
    Expected<int32_t, std::exception> hasValue{ 101 };
    Expected<int32_t, std::exception> hasError{ Unexpected<std::exception>{ std::exception{} } };

    EXPECT_TRUE(hasValue.hasValue());
    EXPECT_TRUE(hasValue);

    EXPECT_FALSE(hasError.hasValue());
    EXPECT_FALSE(hasError);
}

TEST(ExpectedTests, DoesNotCopyValueWhenObserving) {
    struct Helper {
        bool copied{ false };

        Helper() = default;

        Helper(Helper const &other) {
            copied = true;
        }
        Helper(Helper &&other) noexcept = default;

        Helper &operator=(Helper const &other) {
            copied = true;
            return *this;
        }
        Helper &operator=(Helper &&other) noexcept = default;

        ~Helper() = default;
    };

    Expected<Helper, std::exception> expected{ Helper{} };

    EXPECT_FALSE(expected.getValue().copied);
    EXPECT_FALSE(expected->copied);
}

TEST(ExpectedTests, DoesNotCopyErrorWhenObserving) {
    struct Helper {
        bool copied{ false };

        Helper() = default;

        Helper(Helper const &other) {
            copied = true;
        }
        Helper(Helper &&other) noexcept = default;

        Helper &operator=(Helper const &other) {
            copied = true;
            return *this;
        }
        Helper &operator=(Helper &&other) noexcept = default;

        ~Helper() = default;
    };

    Expected<std::string, Helper> expected{ Unexpected<Helper>{ Helper{} } };

    EXPECT_FALSE(expected.getError().copied);
}

TEST(ExpectedTests, MovesValueWhenIsAnRRef) {
    struct Helper {
        bool copied{ false };

        Helper() = default;

        Helper(Helper const &other) {
            copied = true;
        }
        Helper(Helper &&other) noexcept = default;

        Helper &operator=(Helper const &other) {
            copied = true;
            return *this;
        }
        Helper &operator=(Helper &&other) noexcept = default;

        ~Helper() = default;
    };

    Expected<Helper, std::exception> expected{ Helper{} };
    Helper helper{ std::move(expected.getValue()) };

    EXPECT_FALSE(helper.copied);
}

TEST(ExpectedTests, MovesErrorWhenIsAnRRef) {
    struct Helper {
        bool copied{ false };

        Helper() = default;

        Helper(Helper const &other) {
            copied = true;
        }
        Helper(Helper &&other) noexcept = default;

        Helper &operator=(Helper const &other) {
            copied = true;
            return *this;
        }
        Helper &operator=(Helper &&other) noexcept = default;

        ~Helper() = default;
    };

    Expected<std::string, Helper> expected{ Helper{} };
    Helper helper{ std::move(expected.getError()) };

    EXPECT_FALSE(helper.copied);
}

TEST(ExpectedTests, ThrowsWhenAccessingTheValueWhenItIsAnError) {
    Expected<int32_t, std::runtime_error> expected{ std::runtime_error{ "Error!" } };
    Expected<int32_t, std::runtime_error> movedExpected{ std::runtime_error{ "Error!" } };
    Expected<int32_t, std::string> stringAsErrorExpected{ std::string{ "Error!" } };

    EXPECT_ANY_THROW(expected.getValue());
    EXPECT_ANY_THROW(int32_t explicitlyMovedInt{ std::move(movedExpected.getValue()) });
    EXPECT_ANY_THROW(stringAsErrorExpected.getValue());
}

TEST(ExpectedTests, CanReturnProperlyFromAFunction) {
    class NotCopyable {
    public:
        NotCopyable() = default;

        NotCopyable(NotCopyable const &other)     = delete;
        NotCopyable(NotCopyable &&other) noexcept = default;

        NotCopyable &operator=(NotCopyable const &other) = delete;
        NotCopyable &operator=(NotCopyable &&other) noexcept = default;

        ~NotCopyable() = default;
    };

    class ExpectedReturner {
    public:
        Expected<int32_t, std::exception> return42() {
            return 42;
        }

        Expected<int32_t, std::exception> returnException() {
            return Unexpected{ std::exception{} };
        }

        Expected<NotCopyable, std::exception> returnNonCopyable() {
            return NotCopyable{};
        }
    };

    ExpectedReturner helper{};

    auto value{ helper.return42() };
    auto error{ helper.returnException() };
    NotCopyable notCopyableObj{ helper.returnNonCopyable().getValue() };

    EXPECT_TRUE(value.hasValue());
    EXPECT_FALSE(error.hasValue());

    EXPECT_EQ(42, value.getValue());
}