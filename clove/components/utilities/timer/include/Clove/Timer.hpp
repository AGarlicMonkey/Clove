#pragma once

#include <chrono>
#include <fstream>

#if CLOVE_ENABLE_PROFILING
namespace clove {
    struct ProfileResult {
        std::string name;

        int64_t start{ 0 };
        int64_t end{ 0 };

        uint32_t threadID;
    };

    struct TimerSession {
        std::string name;
    };
}

namespace clove {
    class Timer {
        //VARIABLES
    private:
        TimerSession *currentSession = nullptr;
        std::ofstream outputStream;
        uint32_t profileCount = 0;

        //FUNCTIONS
    public:
        Timer();

        ~Timer();

        static Timer &get();

        void beginSession(std::string const &name, std::string const &filepath = "results.json");
        void endSession();

        void writeHeader();
        void writeProfile(ProfileResult const &result);
        void writeFooter();
    };
}

namespace clove {
    class ScopedTimerClock {
        //VARIABLES
    private:
        std::string name;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

        //FUNCTIONS
    public:
        ScopedTimerClock(std::string const &name);

        ~ScopedTimerClock();
    };
}
#endif

#if CLOVE_ENABLE_PROFILING
    #define CLOVE_PROFILE_BEGIN_SESSION(name, filepath) ::clove::Timer::get().beginSession(name, filepath)
    #define CLOVE_PROFILE_END_SESSION() ::clove::Timer::get().endSession()
    #define CLOVE_PROFILE_SCOPE(name) ::clove::ScopedTimerClock timer##__LINE__(name)
    #define CLOVE_PROFILE_FUNCTION() CLOVE_PROFILE_SCOPE(CLOVE_FUNCTION_NAME_PRETTY)
#else
    #define CLOVE_PROFILE_BEGIN_SESSION(name, filepath)
    #define CLOVE_PROFILE_END_SESSION()
    #define CLOVE_PROFILE_SCOPE(name)
    #define CLOVE_PROFILE_FUNCTION()
#endif
