#pragma once

#include <Clove/ECS/Entity.hpp>
#include <Clove/Rendering/Camera.hpp>
#include <Clove/SubSystem.hpp>
#include <msclr/gcroot.h>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace clove {
    class EntityManager;
    namespace reflection {
        class TypeInfo;
    }
}

namespace membrane {
    /**
     * @brief The sub system that controls the interaction between the editor and the engine.
     * Is active while 'editing' and allows the user to build a scene
     */
    class EditorSubSystem : public clove::SubSystem {
        friend ref class EditorSubSystemMessageProxy;

        //VARIABLES
    private:
        msclr::gcroot<EditorSubSystemMessageProxy ^> proxy;

        clove::EntityManager *entityManager{ nullptr };
        std::unordered_map<clove::Entity, std::vector<clove::reflection::TypeInfo const *>> trackedComponents;

        clove::Entity editorCamera{};

        bool moveMouse{ false };
        clove::vec2i mousePos{};
        clove::vec2i prevMousePos{};
        float mouseLookYaw{};
        float mouseLookPitch{};

        //FUNCTIONS
    public:
        EditorSubSystem(clove::EntityManager *entityManager);
        ~EditorSubSystem();

        Group getGroup() const override;

        void onAttach() override;
        clove::InputResponse onInputEvent(clove::InputEvent const &inputEvent) override;
        void onUpdate(clove::DeltaTime const deltaTime) override;
        void onDetach() override;

        void saveScene();
        void loadScene();

    private:
        clove::Entity createEntity(std::string_view name = "New Entity");
        void deleteEntity(clove::Entity entity);

        void addComponent(clove::Entity entity, std::string_view typeName);
        void modifyComponent(clove::Entity entity, std::string_view typeName, size_t offset, std::string_view value);
        void removeComponent(clove::Entity entity, std::string_view typeName);

        void updateName(clove::Entity entity, std::string name);
    };
}