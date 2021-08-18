#pragma once

#include "Clove/Audio/AhaSource.hpp"

#include <AL/al.h>

namespace clove {
    class OpenAlSource : public AhaSource {
        //VARIABLES
    private:
        ALuint source{};

        std::vector<std::shared_ptr<AhaBuffer>> bufferQueue;

        //FUNCTIONS
    public:
        OpenAlSource() = delete;
        OpenAlSource(ALuint source);

        OpenAlSource(OpenAlSource const &other) = delete;
        OpenAlSource(OpenAlSource &&other) noexcept;

        OpenAlSource &operator=(OpenAlSource const &other) = delete;
        OpenAlSource &operator=(OpenAlSource &&other) noexcept;

        ~OpenAlSource();

        void setBuffer(std::shared_ptr<AhaBuffer> buffer) override;

        void queueBuffers(std::vector<std::shared_ptr<AhaBuffer>> buffers) override;
        std::vector<std::shared_ptr<AhaBuffer>> unQueueBuffers(uint32_t const numToUnqueue) override;

        void setPitch(float pitch) override;
        void setLooping(bool isLooping) override;

        void setPosition(vec3f const &position) override;
        void setVelocity(vec3f const &velocity) override;

        vec3f getPosition() const override;
        vec3f getVelocity() const override;

        virtual uint32_t getNumBuffersQueued() const override;
        virtual uint32_t getNumBuffersProcessed() const override;

        void play() override;
        void pause() override;
        void stop() override;
        void rewind() override;
    };
}