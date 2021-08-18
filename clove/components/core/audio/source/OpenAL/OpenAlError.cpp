#include "Clove/Audio/OpenAL/OpenAlError.hpp"

#include <AL/alc.h>
#include <Clove/Log/Log.hpp>

namespace clove {
    void printErrorAl(ALenum error, std::string_view const fileName, uint_fast32_t const line) {
        switch(error) {
            case AL_INVALID_NAME:
                CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "AL_INVALID_NAME: A bad name (id) was passed to an OpenAL function. File: {0} Line {1}", fileName, line);
                break;
            case AL_INVALID_ENUM:
                CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "AL_INVALID_ENUM: An invalid enum value was passed to an OpenAL function. File: {0} Line {1}", fileName, line);
                break;
            case AL_INVALID_VALUE:
                CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "AL_INVALID_VALUE: An invalid value was passed to an OpenAL function. File: {0} Line {1}", fileName, line);
                break;
            case AL_INVALID_OPERATION:
                CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "AL_INVALID_OPERATION: The requested operation was not valid. File: {0} Line {1}", fileName, line);
                break;
            case AL_OUT_OF_MEMORY:
                CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "AL_OUT_OF_MEMORY: The requested operation caused OpenAL to run out of memory. File: {0} Line {1}", fileName, line);
                break;
        }
    }
}