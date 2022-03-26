#pragma once

#include "Membrane/Export.hpp"

#include <wtypes.h>

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
};

extern "C" typedef void(__stdcall *LogSink)(BSTR message);

MEMBRANE_EXPORT void write(LogLevel logLevel, wchar_t const *message);
MEMBRANE_EXPORT void addSink(LogSink sinkFp, wchar_t const *pattern);