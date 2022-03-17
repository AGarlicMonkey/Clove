#include "Membrane/EditorSubSystem.hpp"

#include "Membrane/NameComponent.hpp"

#include <Clove/Application.hpp>
#include <Clove/Components/CameraComponent.hpp>
#include <Clove/Components/TransformComponent.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Maths/MathsHelpers.hpp>
#include <Clove/ModelLoader.hpp>
#include <Clove/Reflection/Reflection.hpp>
#include <Clove/ReflectionAttributes.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <Clove/SubSystems/AudioSubSystem.hpp>
#include <Clove/SubSystems/PhysicsSubSystem.hpp>
#include <Clove/SubSystems/RenderSubSystem.hpp>
#include <Clove/SubSystems/TransformSubSystem.hpp>
#include <Clove/Rendering/Renderer.hpp>

using namespace clove;

CLOVE_DECLARE_LOG_CATEGORY(MembraneEditorSubSystem)

namespace membrane {
    EditorSubSystem::EditorSubSystem(clove::EntityManager *entityManager)
        : clove::SubSystem{ "Editor SubSystem" }
        , entityManager{ entityManager } {
    }

    EditorSubSystem::~EditorSubSystem() = default;

    clove::SubSystem::Group EditorSubSystem::getGroup() const {
        return Group::Core;
    }

    void EditorSubSystem::onAttach() {
        auto &app{ clove::Application::get() };

        entityManager->destroyAll();

        //Make sure we add any default sub systems.
        if(!app.hasSubSystem<RenderSubSystem>()) {
            app.pushSubSystem<RenderSubSystem>(app.getRenderer(), app.getEntityManager());
        }
        if(!app.hasSubSystem<TransformSubSystem>()) {
            app.pushSubSystem<TransformSubSystem>(app.getEntityManager());
        }

        //Add the editor camera outside of the current scene
        editorCamera = entityManager->create();

        bool constexpr makePriority{ true };

        entityManager->addComponent<clove::TransformComponent>(editorCamera).position = clove::vec3f{ 0.0f, 0.0f, -10.0f };
        entityManager->addComponent<clove::CameraComponent>(editorCamera, clove::Camera{ clove::Camera::ProjectionMode::Perspective }, makePriority);

        loadScene();
    }

    clove::InputResponse EditorSubSystem::onInputEvent(clove::InputEvent const &inputEvent) {
        if(auto const *const mouseEvent{ std::get_if<clove::Mouse::Event>(&inputEvent) }) {
            if(mouseEvent->getButton() == clove::MouseButton::Right) {
                if(mouseEvent->getType() == clove::Mouse::Event::Type::Pressed) {
                    //Make sure prev + pos gets updated when the event happens.
                    //This fixes situations where the mouse will be up, move and then go down
                    mousePos     = mouseEvent->getPos();
                    prevMousePos = mouseEvent->getPos();

                    moveMouse = true;
                    return clove::InputResponse::Consumed;
                } else {
                    moveMouse = false;
                    return clove::InputResponse::Consumed;
                }
            }

            if(mouseEvent->getType() == clove::Mouse::Event::Type::Move && moveMouse) {
                mousePos = mouseEvent->getPos();
                return clove::InputResponse::Consumed;
            }
        }
        return clove::InputResponse::Ignored;
    }

    void EditorSubSystem::onUpdate(clove::DeltaTime const deltaTime) {
        auto &keyBoard{ *clove::Application::get().getKeyboard() };
        auto &mouse{ *clove::Application::get().getMouse() };
        auto *const entityManager{ clove::Application::get().getEntityManager() };

        clove::vec3f pos{};
        clove::vec3f rot{};

        //Camera Movement: Keyboard
        if(keyBoard.isKeyPressed(clove::Key::W)) {
            pos.z += 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::S)) {
            pos.z -= 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::A)) {
            pos.x -= 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::D)) {
            pos.x += 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::Space) || keyBoard.isKeyPressed(clove::Key::E)) {
            pos.y += 1.0f;
        }
        if(keyBoard.isKeyPressed(clove::Key::Shift_Left) || keyBoard.isKeyPressed(clove::Key::Q)) {
            pos.y -= 1.0f;
        }

        //Camera Movement: Mouse
        if(moveMouse) {
            float constexpr speed = 10.0f;
            clove::vec2i const delta{ mousePos - prevMousePos };

            mouseLookYaw += static_cast<float>(delta.x) * speed * deltaTime;
            mouseLookPitch += static_cast<float>(delta.y) * speed * deltaTime;

            prevMousePos = mousePos;
        }

        rot.x = clove::asRadians(mouseLookPitch);
        rot.y = clove::asRadians(mouseLookYaw);

        auto &camTrans{ entityManager->getComponent<clove::TransformComponent>(editorCamera) };
        camTrans.rotation = rot;
        camTrans.position += camTrans.rotation * pos * 10.0f * deltaTime.getDeltaSeconds();
    }

    void EditorSubSystem::onDetach() {
        saveScene();
        entityManager->destroyAll();
    }

    void EditorSubSystem::saveScene() {
        serialiser::Node fileNode{};
        serialiser::Node &rootNode{ fileNode["scene"] };

        rootNode["file_version"] = 1;

        for(auto const &subSystem : enabledSubSystems) {
            rootNode["subSystems"].pushBack(subSystem);
        }

        for(auto &&[entity, components] : trackedComponents) {
            serialiser::Node entityNode{};
            entityNode["id"]   = entity;
            entityNode["name"] = entityManager->getComponent<NameComponent>(entity).name;

            for(auto const *typeInfo : components) {
                uint8_t const *const componentMemory{ typeInfo->attributes.get<clove::EditorVisibleComponent>()->onEditorGetComponent(entity, *entityManager) };

                //entityNode["components"][typeInfo->name] = serialiseComponent(typeInfo, componentMemory);
            }

            rootNode["entities"].pushBack(entityNode);
        }

        std::ofstream fileStream{ clove::Application::get().getFileSystem()->resolve("./scene.clvscene"), std::ios::out | std::ios::trunc };
        fileStream << emittYaml(fileNode);
    }

    void EditorSubSystem::loadScene() {
        for(auto &&[entity, components] : trackedComponents) {
            entityManager->destroy(entity);
        }
        trackedComponents.clear();

        std::filesystem::path const scenePath{ clove::Application::get().getFileSystem()->resolve("./scene.clvscene") };
        if(!std::filesystem::exists(scenePath)) {
            CLOVE_LOG(MembraneEditorSubSystem, LogLevel::Warning, "Could not locate 'scene.clvscene' file in game root directory.");
            return;
        }

        auto loadResult{ loadYaml(scenePath) };
        if(!loadResult.hasValue()) {
            if(loadResult.getError() == clove::LoadError::BadFile) {
                CLOVE_LOG(MembraneEditorSubSystem, LogLevel::Error, "Bad scene provided. Scene cannot be loaded.");
            } else {
                CLOVE_LOG(MembraneEditorSubSystem, LogLevel::Error, "Could not load scene file. Error code: {0}", loadResult.getError());
                throw std::runtime_error{ "Scene file could not be loaded. See log for details." };
            }

            return;
        }

        serialiser::Node fileNode{ std::move(loadResult.getValue()) };
        serialiser::Node &rootNode{ fileNode["scene"] };

        //Load sub systems
        //System::Collections::Generic::List<System::String ^> ^ subSystems { gcnew System::Collections::Generic::List<System::String ^> };

        for(auto const &subSystemNode : rootNode["subSystems"]) {
            std::string const subSystemName{ subSystemNode.as<std::string>() };

            enabledSubSystems.push_back(subSystemName);

            //subSystems->Add(gcnew System::String(subSystemName.c_str()));
        }

        //Load entities
        //System::Collections::Generic::List<Entity ^> ^ entities { gcnew System::Collections::Generic::List<Entity ^> };

        for(auto const &entityNode : rootNode["entities"]) {
            clove::Entity entity{ entityManager->create() };
            //Entity ^ editorEntity { gcnew Entity };

            std::string const name{ entityNode["name"].as<std::string>() };

            entityManager->addComponent<NameComponent>(entity, name);

            // editorEntity->id         = entity;
            // editorEntity->name       = gcnew System::String{ name.c_str() };
            // editorEntity->components = gcnew System::Collections::Generic::List<EditorTypeInfo ^>;

            for(auto const &componentNode : entityNode["components"]) {
                std::string const componentName{ componentNode.getKey() };
                clove::reflection::TypeInfo const *const typeInfo{ clove::reflection::getTypeInfo(componentName) };

                if(typeInfo == nullptr) {
                    CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "Could not get typeinfo for {0}", componentName);
                    continue;
                }

                uint8_t *const componentMemory{ typeInfo->attributes.get<clove::EditorVisibleComponent>()->onEditorCreateComponent(entity, *entityManager) };
                //deserialiseComponent(typeInfo, componentMemory, componentNode);

                trackedComponents[entity].push_back(typeInfo);
                //editorEntity->components->Add(constructComponentEditorTypeInfo(typeInfo, componentMemory));
            }

            //entities->Add(editorEntity);
        }

        // Engine_OnSceneLoaded ^ message { gcnew Engine_OnSceneLoaded };
        // message->enabledSubSystems = subSystems;
        // message->entities          = entities;
        // MessageHandler::sendMessage(message);
    }

    clove::Entity EditorSubSystem::createEntity(std::string_view name) {
        clove::Entity entity{ entityManager->create() };
        entityManager->addComponent<NameComponent>(entity, std::string{ std::move(name) });

        // Engine_OnEntityCreated ^ message { gcnew Engine_OnEntityCreated };
        // message->entity = entity;
        // message->name   = gcnew System::String(std::string{ std::begin(name), std::end(name) }.c_str());
        // MessageHandler::sendMessage(message);

        return entity;
    }

    void EditorSubSystem::deleteEntity(clove::Entity entity) {
        entityManager->destroy(entity);

        // Engine_OnEntityDeleted ^ message { gcnew Engine_OnEntityDeleted };
        // message->entity = entity;
        // MessageHandler::sendMessage(message);

        trackedComponents.erase(entity);
    }

    void EditorSubSystem::addSubSystem(std::string name) {
        enabledSubSystems.push_back(name);
    }

    void EditorSubSystem::removeSubSystem(std::string name) {
        enabledSubSystems.erase(std::remove(enabledSubSystems.begin(), enabledSubSystems.end(), name), enabledSubSystems.end());
    }

    void EditorSubSystem::addComponent(clove::Entity entity, std::string_view typeName) {
        clove::reflection::TypeInfo const *typeInfo{ clove::reflection::getTypeInfo(typeName) };
        if(typeInfo == nullptr) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "Could not get typeinfo for {0}", typeName);
            return;
        }

        std::optional<clove::EditorVisibleComponent> componentAttribute{ typeInfo->attributes.get<clove::EditorVisibleComponent>() };
        if(!componentAttribute.has_value()) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "Could not get EditorVisibleComponent attribute for {0}", typeName);
            return;
        }

        if(componentAttribute->onEditorCreateComponent == nullptr) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "{0} does not provide an implementation for EditorVisibleComponent::onEditorCreateComponent. Cannot create component", typeName);
            return;
        }

        trackedComponents[entity].push_back(typeInfo);

        uint8_t *componentMemory{ componentAttribute->onEditorCreateComponent(entity, *entityManager) };

        // auto message{ gcnew Engine_OnComponentAdded };
        // message->entity        = entity;
        // message->componentName = gcnew System::String{ typeName.data() };
        // message->typeInfo      = constructComponentEditorTypeInfo(typeInfo, componentMemory);

        // MessageHandler::sendMessage(message);
    }

    void EditorSubSystem::modifyComponent(clove::Entity entity, std::string_view typeName, size_t offset, std::string_view value) {
        clove::reflection::TypeInfo const *typeInfo{ clove::reflection::getTypeInfo(typeName) };
        if(typeInfo == nullptr) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "Could not get typeinfo for {0}", typeName);
            return;
        }

        std::optional<clove::EditorVisibleComponent> componentAttribute{ typeInfo->attributes.get<clove::EditorVisibleComponent>() };
        if(!componentAttribute.has_value()) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "Could not get EditorVisibleComponent attribute for {0}", typeName);
            return;
        }

        if(componentAttribute->onEditorGetComponent == nullptr) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "{0} does not provide an implementation for EditorVisibleComponent::onEditorGetComponent. Cannot modify component", typeName);
            return;
        }

        uint8_t *componentMemory{ componentAttribute->onEditorGetComponent(entity, *entityManager) };

        size_t startingOffset{ 0 };
        //modifyComponentMember(componentMemory, typeInfo, value, offset, startingOffset);
    }

    void EditorSubSystem::removeComponent(clove::Entity entity, std::string_view typeName) {
        clove::reflection::TypeInfo const *typeInfo{ clove::reflection::getTypeInfo(typeName) };
        if(typeInfo == nullptr) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "Could not get typeinfo for {0}", typeName);
            return;
        }

        std::optional<clove::EditorVisibleComponent> componentAttribute{ typeInfo->attributes.get<clove::EditorVisibleComponent>() };
        if(!componentAttribute.has_value()) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "Could not get EditorVisibleComponent attribute for {0}", typeName);
            return;
        }

        if(componentAttribute->onEditorCreateComponent == nullptr) {
            CLOVE_LOG(MembraneEditorSubSystem, clove::LogLevel::Error, "{0} does not provide an implementation for EditorVisibleComponent::onEditorCreateComponent. Cannot remove component", typeName);
            return;
        }

        auto &componentArray{ trackedComponents.at(entity) };
        componentArray.erase(std::remove(componentArray.begin(), componentArray.end(), typeInfo));

        componentAttribute->onEditorDestroyComponent(entity, *entityManager);

        // auto message{ gcnew Engine_OnComponentRemoved };
        // message->entity        = entity;
        // message->componentName = gcnew System::String{ typeName.data() };
        // MessageHandler::sendMessage(message);
    }

    void EditorSubSystem::updateName(clove::Entity entity, std::string name) {
        if(entityManager->hasComponent<NameComponent>(entity)) {
            entityManager->getComponent<NameComponent>(entity).name = std::move(name);
        }
    }
}