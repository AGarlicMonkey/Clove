#include "Membrane/Log.hpp"

#include <Clove/Log/Log.hpp>
#include <codecvt>
#include <spdlog/details/log_msg.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/sink.h>

//Making the assumption that this library will only be used with Bulb
CLOVE_DECLARE_LOG_CATEGORY(Bulb);

namespace {
    clove::LogLevel convertLevel(LogLevel level) {
        switch(level) {
            default:
            case LogLevel::Trace:
                return clove::LogLevel::Trace;
            case LogLevel::Debug:
                return clove::LogLevel::Debug;
            case LogLevel::Info:
                return clove::LogLevel::Info;
            case LogLevel::Warning:
                return clove::LogLevel::Warning;
            case LogLevel::Error:
                return clove::LogLevel::Error;
            case LogLevel::Critical:
                return clove::LogLevel::Critical;
        }
    }

    class FunctionPointerSink : public spdlog::sinks::sink {
        //VARIABLES
    private:
        LogSink sinkFp{ nullptr };
        std::unique_ptr<spdlog::formatter> formatter;

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter{};

        //FUNCTIONS
    public:
        FunctionPointerSink(LogSink sinkFp)
            : sinkFp{ sinkFp } {
        }

        void log(spdlog::details::log_msg const &msg) override {
            spdlog::memory_buf_t formatted;
            formatter->format(msg, formatted);

            std::wstring const wideMsg{ stringConverter.from_bytes(std::string{ formatted.data(), formatted.size() }) };
            sinkFp(SysAllocString(wideMsg.c_str()));
        }

        void flush() override {}

        void set_pattern(std::string const &pattern) override {
            set_formatter(spdlog::details::make_unique<spdlog::pattern_formatter>(pattern));
        }

        void set_formatter(std::unique_ptr<spdlog::formatter> formatter) override {
            this->formatter = std::move(formatter);
        }
    };
}

void write(LogLevel logLevel, wchar_t const *message) {
    std::string const narrowMsg{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(message) };

    CLOVE_LOG(Bulb, convertLevel(logLevel), narrowMsg);
}

void addSink(LogSink sinkFp, wchar_t const *pattern) {
    std::string const narrowPattern{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(pattern) };

    auto sink{ std::make_shared<FunctionPointerSink>(sinkFp) };
    sink->set_pattern(narrowPattern);
    sink->set_level(spdlog::level::debug);

    clove::Logger::get().addSink(std::move(sink));
}