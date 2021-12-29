#include "Membrane/EditorSubSystem.hpp"

#include "Membrane/MembraneLog.hpp"
#include "Membrane/MessageHandler.hpp"
#include "Membrane/Messages.hpp"
#include "Membrane/NameComponent.hpp"
#include "Membrane/ReflectionHelper.hpp"

#include <Clove/Application.hpp>
#include <Clove/Components/CameraComponent.hpp>
#include <Clove/Components/CollisionShapeComponent.hpp>
#include <Clove/Components/PointLightComponent.hpp>
#include <Clove/Components/RigidBodyComponent.hpp>
#include <Clove/Components/StaticModelComponent.hpp>
#include <Clove/Components/TransformComponent.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Maths/MathsHelpers.hpp>
#include <Clove/ModelLoader.hpp>
#include <Clove/Reflection/Reflection.hpp>
#include <Clove/ReflectionAttributes.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <Clove/SubSystems/PhysicsSubSystem.hpp>
#include <msclr/marshal_cppstd.h>

using namespace clove;

namespace {
    System::Collections::Generic::List<membrane::EditorTypeInfo ^> ^ constructMembers(std::vector<reflection::MemberInfo> const &members, void const *const memory, size_t offsetIntoParent) {
        auto editorVisibleMemers{ gcnew System::Collections::Generic::List<membrane::EditorTypeInfo ^>{} };

        for(auto const &member : members) {
            if(std::optional<EditorEditableMember> attribute{ member.attributes.get<EditorEditableMember>() }) {
                size_t const totalMemberOffset{ offsetIntoParent + member.offset };

                auto memberInfo{ gcnew membrane::EditorTypeInfo{} };
                memberInfo->typeName    = gcnew System::String{ member.name.c_str() };
                memberInfo->displayName = gcnew System::String{ attribute->name.value_or(member.name).c_str() };
                memberInfo->offset      = totalMemberOffset;
                if(reflection::TypeInfo const *memberTypeInfo{ reflection::getTypeInfo(member.id) }) {
                    memberInfo->members = constructMembers(memberTypeInfo->members, memory, totalMemberOffset);
                    memberInfo->value   = nullptr;
                } else {
                    memberInfo->members = nullptr;

                    if(attribute->onEditorGetValue != nullptr) {
                        std::string value{ attribute->onEditorGetValue(reinterpret_cast<uint8_t const *const>(memory), totalMemberOffset, member.size) };
                        memberInfo->value = gcnew System::String{ value.c_str() };
                    } else {
                        CLOVE_LOG(Membrane, clove::LogLevel::Error, "EditorEditableMember {0} does not provide an implemntation for onEditorGetValue", member.name);
                    }
                }

                editorVisibleMemers->Add(memberInfo);
            }
        }

        return editorVisibleMemers;
    }

    membrane::EditorTypeInfo^ constructEditorTypeInfo(reflection::TypeInfo const *typeInfo, void const *const memory) {
        EditorVisibleComponent const visibleAttribute{ typeInfo->attributes.get<EditorVisibleComponent>().value() };

        auto editorTypeInfo{ gcnew membrane::EditorTypeInfo{} };
        editorTypeInfo->typeName    = gcnew System::String{ typeInfo->name.c_str() };
        editorTypeInfo->displayName = gcnew System::String{ visibleAttribute.name.value_or(typeInfo->name).c_str() };
        editorTypeInfo->members     = constructMembers(typeInfo->members, memory, 0);

        return editorTypeInfo;
    }

    void modifyComponentMember(uint8_t *const memory, clove::reflection::TypeInfo const *typeInfo, std::string_view value, size_t const requiredOffset, size_t currentOffset) {
        for(auto const &member : typeInfo->members) {
            if(std::optional<EditorEditableMember> attribute{ member.attributes.get<EditorEditableMember>() }) {
                size_t const totalMemberOffset{ currentOffset + member.offset };
                reflection::TypeInfo const *memberTypeInfo{ reflection::getTypeInfo(member.id) };

                if(memberTypeInfo != nullptr) {
                    modifyComponentMember(memory, memberTypeInfo, value, requiredOffset, totalMemberOffset);
                } else if(totalMemberOffset == requiredOffset) {
                    attribute->onEditorSetValue(memory, totalMemberOffset, member.size, value);
                }
            }
        }
    }

    serialiser::Node serialiseComponent(reflection::TypeInfo const *const componentTypeInfo, uint8_t const *const componentMemory, size_t currentOffset = 0) {
        if(componentTypeInfo == nullptr) {
            return {};
        }

        serialiser::Node members{};

        for(auto const &member : componentTypeInfo->members) {
            if(std::optional<EditorEditableMember> attribute{ member.attributes.get<EditorEditableMember>() }) {
                size_t const totalMemberOffset{ currentOffset + member.offset };

                if(reflection::TypeInfo const *memberType{ reflection::getTypeInfo(member.id) }) {
                    members[member.name] = serialiseComponent(memberType, componentMemory, totalMemberOffset);
                } else {
                    members[member.name] = attribute->onEditorGetValue(componentMemory, totalMemberOffset, member.size);
                }
            }
        }

        return members;
    }
}

namespace membrane {
    // clang-format off
    /**
     * @brief 
     */
    private ref class EditorSubSystemMessageProxy {
        //VARIABLES
    private:
        EditorSubSystem *subSystem{ nullptr };

        //FUNCTIONS
    public:
        EditorSubSystemMessageProxy(EditorSubSystem *layer)
            : subSystem{ layer } {
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_CreateEntity ^>(this, &EditorSubSystemMessageProxy::createEntity));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_DeleteEntity ^>(this, &EditorSubSystemMessageProxy::deleteEntity));

            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_AddComponent ^>(this, &EditorSubSystemMessageProxy::addComponent));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_ModifyComponent ^>(this, &EditorSubSystemMessageProxy::modifyComponent));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_RemoveComponent ^>(this, &EditorSubSystemMessageProxy::removeComponent));

            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_UpdateName ^>(this, &EditorSubSystemMessageProxy::updateName));

            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_SaveScene ^>(this, &EditorSubSystemMessageProxy::saveScene));
            MessageHandler::bindToMessage(gcnew MessageSentHandler<Editor_LoadScene ^>(this, &EditorSubSystemMessageProxy::loadScene));
        }

    public:
        void reset(){
            subSystem = nullptr;
        }

    private:
        void createEntity(Editor_CreateEntity ^ message) {
            if (subSystem){
                subSystem->createEntity();
            }
        }

        void deleteEntity(Editor_DeleteEntity ^ message) {
            if (subSystem){
                clove::Entity const entity{ message->entity };
                subSystem->deleteEntity(entity);
            }
        }

        void addComponent(Editor_AddComponent ^ message){
            if (subSystem){
                System::String ^typeName{ message->componentName };
                subSystem->addComponent(message->entity, msclr::interop::marshal_as<std::string>(typeName));
            }
        }

        void modifyComponent(Editor_ModifyComponent ^ message){
            if (subSystem){
                System::String ^typeName{ message->componentName };
                System::String ^memberValue{ message->value };
                subSystem->modifyComponent(message->entity, msclr::interop::marshal_as<std::string>(typeName), message->offset, msclr::interop::marshal_as<std::string>(memberValue));
            }
        }

        void removeComponent(Editor_RemoveComponent ^ message){
            if (subSystem){
                System::String ^typeName{ message->componentName };
                subSystem->removeComponent(message->entity, msclr::interop::marshal_as<std::string>(typeName));
            }
        }

        void updateName(Editor_UpdateName ^ message){
            if (subSystem){
                System::String ^name{ message->name };
                subSystem->updateName(message->entity, msclr::interop::marshal_as<std::string>(name));
            }
        }

        void saveScene(Editor_SaveScene ^message){
            if (subSystem){
                subSystem->saveScene();
            }
        }

        void loadScene(Editor_LoadScene ^message){
            if (subSystem){
                subSystem->loadScene();
            }
        }
    };
    // clang-format on
}

namespace membrane {
    EditorSubSystem::EditorSubSystem(clove::EntityManager *entityManager)
        : clove::SubSystem{ "Editor SubSystem" }
        , entityManager{ entityManager } {
        proxy = gcnew EditorSubSystemMessageProxy(this);
    }

    EditorSubSystem::~EditorSubSystem() {
        proxy->reset();
    }

    clove::SubSystem::Group EditorSubSystem::getGroup() const {
        return Group::Core;
    }

    void EditorSubSystem::onAttach() {
        auto &app{ clove::Application::get() };

        //Add the editor camera outside of the current scene
        if(editorCamera == clove::NullEntity) {
            auto *const entityManager{ app.getEntityManager() };

            editorCamera                                                                  = entityManager->create();
            entityManager->addComponent<clove::TransformComponent>(editorCamera).position = clove::vec3f{ 0.0f, 0.0f, -10.0f };
            entityManager->addComponent<clove::CameraComponent>(editorCamera, clove::Camera{ clove::Camera::ProjectionMode::Perspective });
        }

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
        serialiser::Node rootNode{};
        for(auto &&[entity, components] : trackedComponents) {
            serialiser::Node entityNode{};
            entityNode["id"]   = entity;
            entityNode["name"] = entityManager->getComponent<NameComponent>(entity).name;

            for(auto const *typeInfo : components) {
                uint8_t const *const componentMemory{ typeInfo->attributes.get<clove::EditorVisibleComponent>()->onEditorGetComponent(entity, *entityManager) };

                entityNode["components"][typeInfo->name] = serialiseComponent(typeInfo, componentMemory);
            }

            rootNode["entities"].pushBack(entityNode);
        }

        std::ofstream fileStream{ clove::Application::get().getFileSystem()->resolve("./scene.clvscene"), std::ios::out | std::ios::trunc };
        fileStream << emittYaml(rootNode);
    }

    void EditorSubSystem::loadScene() {
    }

    clove::Entity EditorSubSystem::createEntity(std::string_view name) {
        clove::Entity entity{ entityManager->create() };
        entityManager->addComponent<NameComponent>(entity, std::string{ std::move(name) });

        Engine_OnEntityCreated ^ message { gcnew Engine_OnEntityCreated };
        message->entity = entity;
        message->name   = gcnew System::String(std::string{ std::begin(name), std::end(name) }.c_str());
        MessageHandler::sendMessage(message);

        return entity;
    }

    void EditorSubSystem::deleteEntity(clove::Entity entity) {
        entityManager->destroy(entity);

        Engine_OnEntityDeleted ^ message { gcnew Engine_OnEntityDeleted };
        message->entity = entity;
        MessageHandler::sendMessage(message);
    }

    void EditorSubSystem::addComponent(clove::Entity entity, std::string_view typeName) {
        clove::reflection::TypeInfo const *typeInfo{ clove::reflection::getTypeInfo(typeName) };
        if(typeInfo == nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get typeinfo for {0}", typeName);
            return;
        }

        std::optional<clove::EditorVisibleComponent> componentAttribute{ typeInfo->attributes.get<clove::EditorVisibleComponent>() };
        if(!componentAttribute.has_value()) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get EditorVisibleComponent attribute for {0}", typeName);
            return;
        }

        if(componentAttribute->onEditorCreateComponent == nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "{0} does not provide an implementation for EditorVisibleComponent::onEditorCreateComponent. Cannot create component", typeName);
            return;
        }

        trackedComponents[entity].push_back(typeInfo);

        uint8_t *componentMemory{ componentAttribute->onEditorCreateComponent(entity, *entityManager) };

        auto message{ gcnew Engine_OnComponentAdded };
        message->entity        = entity;
        message->componentName = gcnew System::String{ typeName.data() };
        message->typeInfo      = constructEditorTypeInfo(typeInfo, componentMemory);

        MessageHandler::sendMessage(message);
    }

    void EditorSubSystem::modifyComponent(clove::Entity entity, std::string_view typeName, size_t offset, std::string_view value) {
        clove::reflection::TypeInfo const *typeInfo{ clove::reflection::getTypeInfo(typeName) };
        if(typeInfo == nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get typeinfo for {0}", typeName);
            return;
        }

        std::optional<clove::EditorVisibleComponent> componentAttribute{ typeInfo->attributes.get<clove::EditorVisibleComponent>() };
        if(!componentAttribute.has_value()) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get EditorVisibleComponent attribute for {0}", typeName);
            return;
        }

        if(componentAttribute->onEditorGetComponent == nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "{0} does not provide an implementation for EditorVisibleComponent::onEditorGetComponent. Cannot modify component", typeName);
            return;
        }

        uint8_t *componentMemory{ componentAttribute->onEditorGetComponent(entity, *entityManager) };

        size_t startingOffset{ 0 };
        modifyComponentMember(componentMemory, typeInfo, value, offset, startingOffset);
    }

    void EditorSubSystem::removeComponent(clove::Entity entity, std::string_view typeName) {
        clove::reflection::TypeInfo const *typeInfo{ clove::reflection::getTypeInfo(typeName) };
        if(typeInfo == nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get typeinfo for {0}", typeName);
            return;
        }

        std::optional<clove::EditorVisibleComponent> componentAttribute{ typeInfo->attributes.get<clove::EditorVisibleComponent>() };
        if(!componentAttribute.has_value()) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "Could not get EditorVisibleComponent attribute for {0}", typeName);
            return;
        }

        if(componentAttribute->onEditorCreateComponent == nullptr) {
            CLOVE_LOG(Membrane, clove::LogLevel::Error, "{0} does not provide an implementation for EditorVisibleComponent::onEditorCreateComponent. Cannot remove component", typeName);
            return;
        }

        auto &componentArray{ trackedComponents.at(entity) };
        componentArray.erase(std::remove(componentArray.begin(), componentArray.end(), typeInfo));

        componentAttribute->onEditorDestroyComponent(entity, *entityManager);

        auto message{ gcnew Engine_OnComponentRemoved };
        message->entity        = entity;
        message->componentName = gcnew System::String{ typeName.data() };
        MessageHandler::sendMessage(message);
    }

    void EditorSubSystem::updateName(clove::Entity entity, std::string name) {
        if(entityManager->hasComponent<NameComponent>(entity)) {
            entityManager->getComponent<NameComponent>(entity).name = std::move(name);
        }
    }
}