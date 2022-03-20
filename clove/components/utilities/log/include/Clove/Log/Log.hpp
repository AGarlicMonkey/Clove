#pragma once

#include <Clove/Definitions.hpp>
#include <memory>
#include <string>
#include <string_view>

namespace spdlog {
    class logger;
    namespace sinks {
        class sink;
    }
}

namespace clove {
    enum class LogLevel {
        Trace,    /**< Printed to the log file only. */
        Debug,    /**< Lowest log level that will be printed to console. */
        Info,     /**< Useful information to present to the user. */
        Warning,  /**< When something happened that might cause an error. */
        Error,    /**< Something has gone wrong but is recoverable. */
        Critical, /**< Someithng has gone wrong and the application needs to exit. */
    };
}

namespace clove {
    class Logger {
        //VARIABLES
    private:
        std::unique_ptr<spdlog::logger> logger;

        //FUNCTIONS
    public:
        Logger();
        ~Logger();

        static Logger &get();

        template<typename... Args>
        void log(std::string_view category, LogLevel level, std::string_view msg, Args &&...args);

        void addSink(std::shared_ptr<spdlog::sinks::sink> sink);

    private:
        void doLog(LogLevel level, std::string_view msg);
    };
}

#define CLOVE_EXPAND_CATEGORY(category) LOG_CATEGORY_##category

#define CLOVE_DECLARE_LOG_CATEGORY(categoryName)                 \
    struct CLOVE_EXPAND_CATEGORY(categoryName) {                 \
        static std::string_view constexpr name{ #categoryName }; \
    };

#define CLOVE_LOG(category, level, ...) ::clove::Logger::get().log(CLOVE_EXPAND_CATEGORY(category)::name, level, __VA_ARGS__);

#if CLOVE_ENABLE_ASSERTIONS
CLOVE_DECLARE_LOG_CATEGORY(CloveAssert)

    /**
     * @brief Will cause the application to break if the condition is false.
     * @details This macro can be compiled out if CLOVE_ENABLE_ASSERTIONS is 0. When this happens
     * any code inside the macro will also be removed. To do an assertion with a side effect see CLOVE_VERIFY.
     */
    #define CLOVE_ASSERT(x)                                                                                                  \
        {                                                                                                                    \
            if(!(x)) {                                                                                                       \
                CLOVE_LOG(CloveAssert, ::clove::LogLevel::Critical, "Assertion failed: {0}", #x);                            \
                CLOVE_LOG(CloveAssert, ::clove::LogLevel::Critical, "Assertion function:\n{0}", CLOVE_FUNCTION_NAME_PRETTY); \
                CLOVE_DEBUG_BREAK;                                                                                           \
            }                                                                                                                \
        }

    /**
     * @copydoc CLOVE_ASSERT
     */
    #define CLOVE_ASSERT_MSG(x, ...)                                                                    \
        {                                                                                               \
            if(!(x)) {                                                                                  \
                CLOVE_LOG(CloveAssert, ::clove::LogLevel::Critical, "Assertion failed: {0}", #x);       \
                CLOVE_LOG(CloveAssert, ::clove::LogLevel::Critical, "Assertion message: " __VA_ARGS__); \
                CLOVE_DEBUG_BREAK;                                                                      \
            }                                                                                           \
        }

    /**
     * @brief Will cause the application to break if the condition is false.
     * @details This macro can be compiled out if CLOVE_ENABLE_ASSERTIONS is 0. However, code inside the macro
     * will still be executed if this is the case. To compile out the code as well see CLOVE_ASSERT
     */
    #define CLOVE_VERIFY(x) CLOVE_ASSERT(x)

    /**
     * @copydoc CLOVE_VERIFY
     */
    #define CLOVE_VERIFY_MSG(x, ...) CLOVE_ASSERT_MSG(x)
#else
    /**
     * @copydoc CLOVE_ASSERT
     */
    #define CLOVE_ASSERT(x)
    /**
     * @copydoc CLOVE_ASSERT
     */
    #define CLOVE_ASSERT_MSG(x, ...)
    /**
     * @copydoc CLOVE_VERIFY
     */
    #define CLOVE_VERIFY(x) x
    /**
     * @copydoc CLOVE_VERIFY
     */
    #define CLOVE_VERIFY_MSG(x, ...) x
#endif

#include "Log.inl"