#include "Clove/SubSystems/AudioSubSystem.hpp"

#include "Clove/Application.hpp"
#include "Clove/Components/AudioListenerComponent.hpp"
#include "Clove/Components/AudioSourceComponent.hpp"
#include "Clove/Components/TransformComponent.hpp"

#include <Clove/Audio/AhaListener.hpp>
#include <Clove/Audio/AhaSource.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/ReflectionAttributes.hpp>

namespace clove {
    AudioSubSystem::AudioSubSystem()
        : SubSystem("Audio")
        , entityManager{ Application::get().getEntityManager() } {
    }

    AudioSubSystem::AudioSubSystem(AudioSubSystem &&other) noexcept = default;

    AudioSubSystem &AudioSubSystem::operator=(AudioSubSystem &&other) noexcept = default;

    AudioSubSystem::~AudioSubSystem() = default;

    SubSystem::Group AudioSubSystem::getGroup() const {
        return Group::Render;
    }

    void AudioSubSystem::onUpdate(DeltaTime const deltaTime) {
        CLOVE_PROFILE_FUNCTION();

        //Listener
        entityManager->forEach([](TransformComponent const &transform, AudioListenerComponent &listener) {
            vec3f const prevPos{ listener.listener->getPosition() };
            vec3f const currPos{ transform.position };

            listener.listener->setPosition(currPos);
            listener.listener->setVelocity(currPos - prevPos);
        });

        //Source
        entityManager->forEach([](TransformComponent const &transform, AudioSourceComponent &source) {
            vec3f const prevPos{ source.source->getPosition() };
            vec3f const currPos{ transform.position };

            source.source->setPosition(currPos);
            source.source->setVelocity(currPos - prevPos);
        });
    }
}

CLOVE_REFLECT_BEGIN(clove::AudioSubSystem, clove::EditorVisibleSubSystem{
                                               .onEditorCreateSubSystem  = &createSubSystemHelper<clove::AudioSubSystem>,
                                               .onEditorDestroySubSystem = &destroySubSystemHelper<clove::AudioSubSystem>,
                                           })
CLOVE_REFLECT_END