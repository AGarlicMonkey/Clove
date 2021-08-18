#include <spdlog/fmt/fmt.h>

namespace clove {
    Logger &Logger::get(){
        static Logger *instance{ nullptr };

        if(instance == nullptr){
            instance = new Logger;
        }

        return *instance;
    }

    template<typename... Args>
    void Logger::log(std::string_view category, LogLevel level, std::string_view msg, Args &&... args) {
        std::string_view constexpr fullMessageFormat = "{}: {}";

        fmt::basic_memory_buffer<char, 250> messageBuffer;
        fmt::format_to(messageBuffer, msg, std::forward<Args>(args)...);

        fmt::basic_memory_buffer<char, 250> fullMessageBuffer;
        fmt::format_to(fullMessageBuffer, fullMessageFormat, category, std::string_view(messageBuffer.data(), messageBuffer.size()));

        doLog(level, std::string_view(fullMessageBuffer.data(), fullMessageBuffer.size()));
    }
}