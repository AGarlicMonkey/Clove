#include "Membrane/RuntimeSubSystem.hpp"

#include "Membrane/ReflectionHelpers.hpp"
#include "Membrane/MembraneLog.hpp"

#include <Clove/ReflectionAttributes.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Application.hpp>
#include <Clove/Serialisation/Yaml.hpp>

using namespace clove;

namespace membrane {
    RuntimeSubSystem::RuntimeSubSystem(EntityManager *manager)
        : SubSystem{ "Runtime Sub system" }
        , entityManager{ manager } {
    }

    RuntimeSubSystem::RuntimeSubSystem(RuntimeSubSystem &&other) noexcept = default;

    RuntimeSubSystem &RuntimeSubSystem::operator=(RuntimeSubSystem &&other) noexcept = default;

    RuntimeSubSystem::~RuntimeSubSystem() = default;

    SubSystem::Group RuntimeSubSystem::getGroup() const {
        return Group::Core;
    }

    void RuntimeSubSystem::onAttach() {
        auto loadResult{ loadYaml(clove::Application::get().getFileSystem()->resolve("./scene.clvscene")) };
        serialiser::Node rootNode{ loadResult.getValue() };

        //Load sub systems
        for(auto const &subSystemNode : rootNode["subSystems"]) {
            std::string const subSystemName{ subSystemNode.as<std::string>() };
            clove::reflection::TypeInfo const *const typeInfo{ clove::reflection::getTypeInfo(subSystemName) };

            if(typeInfo == nullptr) {
                CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get typeinfo for {0}", subSystemName);
                continue;
            }

            typeInfo->attributes.get<EditorVisibleSubSystem>()->onEditorCreateSubSystem(clove::Application::get());

            trackedSubSystems.push_back(typeInfo);
        }

        //Load entities
        for(auto const &entityNode : rootNode["entities"]) {
            clove::Entity entity{ entityManager->create() };

            for(auto const &componentNode : entityNode["components"]) {
                std::string const componentName{ componentNode.getKey() };
                clove::reflection::TypeInfo const *const typeInfo{ clove::reflection::getTypeInfo(componentName) };

                if(typeInfo == nullptr) {
                    CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get typeinfo for {0}", componentName);
                    continue;
                }

                uint8_t *const componentMemory{ typeInfo->attributes.get<clove::EditorVisibleComponent>()->onEditorCreateComponent(entity, *entityManager) };
                deserialiseComponent(typeInfo, componentMemory, componentNode);
            }
        }
    }

    clove::InputResponse RuntimeSubSystem::onInputEvent(clove::InputEvent const &inputEvent) {
        return clove::InputResponse::Ignored;
    }

    void RuntimeSubSystem::onUpdate(clove::DeltaTime const deltaTime) {
    }

    void RuntimeSubSystem::onDetach() {
        for(auto const *const subSystemInfo : trackedSubSystems) {
            subSystemInfo->attributes.get<EditorVisibleSubSystem>()->onEditorDestroySubSystem(clove::Application::get());
        }

        entityManager->destroyAll();
    }
}
