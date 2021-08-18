#include "Clove/Audio/OpenAL/OpenAlListener.hpp"

#include "Clove/Audio/OpenAL/OpenAlError.hpp"

namespace clove {
    OpenAlListener::OpenAlListener() = default;

    OpenAlListener::OpenAlListener(OpenAlListener &&other) noexcept = default;

    OpenAlListener &OpenAlListener::operator=(OpenAlListener &&other) noexcept = default;

    OpenAlListener::~OpenAlListener() = default;

    void OpenAlListener::setPosition(vec3f const &position) {
        alCall(alListener3f(AL_POSITION, position.x, position.y, position.z));
    }

    void OpenAlListener::setVelocity(vec3f const &velocity) {
        alCall(alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z));
    }

    vec3f OpenAlListener::getPosition() const {
        vec3f position{};
        alCall(alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z));
        return position;
    }

    vec3f OpenAlListener::getVelocity() const {
        vec3f velocity{};
        alCall(alGetListener3f(AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z));
        return velocity;
    }
}