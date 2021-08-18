#pragma once

#include <AL/al.h>
#include <string_view>

namespace clove {
    void printErrorAl(ALenum error, std::string_view const fileName, uint_fast32_t const line);
}

#define alCall(call)                                 \
    call;                                            \
    {                                                \
        ALenum error = alGetError();                 \
        if(error != AL_NO_ERROR) {                   \
            printErrorAl(error, __FILE__, __LINE__); \
        }                                            \
    }
