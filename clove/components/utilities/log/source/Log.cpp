#include "Clove/Log/Log.hpp"

#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace clove {
    Logger::Logger() {
        auto consoleSink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
        auto fileSink{ std::make_shared<spdlog::sinks::basic_file_sink_mt>("CLOVE_LOG.txt", true) };

        consoleSink->set_pattern("%^[%T] %v%$");
        fileSink->set_pattern("[%D %T][%l] %v");

        consoleSink->set_level(spdlog::level::debug);
        fileSink->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> sinks{ consoleSink, fileSink };
        logger = std::make_unique<spdlog::logger>("GARLIC_LOGGER", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
    }

    Logger::~Logger() = default;

    void Logger::addSink(std::shared_ptr<spdlog::sinks::sink> sink) {
        logger->sinks().push_back(std::move(sink));
    }

    void Logger::doLog(LogLevel level, std::string_view msg) {
        switch(level) {
            case LogLevel::Trace:
                logger->trace(msg);
                break;
            case LogLevel::Debug:
                logger->debug(msg);
                break;
            case LogLevel::Info:
                logger->info(msg);
                break;
            case LogLevel::Warning:
                logger->warn(msg);
                break;
            case LogLevel::Error:
                logger->error(msg);
                break;
            case LogLevel::Critical:
                logger->critical(msg);
                break;
        }
    }
}