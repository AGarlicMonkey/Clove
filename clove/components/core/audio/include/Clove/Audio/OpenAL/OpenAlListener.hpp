#pragma once

#include "Clove/Audio/AhaListener.hpp"

#include <AL/al.h>

namespace clove {
    class OpenAlListener : public AhaListener {
        //FUNCTIONS
    public:
        OpenAlListener();

        OpenAlListener(OpenAlListener const &other) = delete;
        OpenAlListener(OpenAlListener &&other) noexcept;

        OpenAlListener &operator=(OpenAlListener const &other) = delete;
        OpenAlListener &operator=(OpenAlListener &&other) noexcept;

        ~OpenAlListener();

        void setPosition(vec3f const &position) override;
        void setVelocity(vec3f const &velocity) override;

        vec3f getPosition() const override;
        vec3f getVelocity() const override;
    };
}