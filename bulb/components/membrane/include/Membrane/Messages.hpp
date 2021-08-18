#pragma once

#include "Membrane/EditorTypes.hpp"
#include "Membrane/MessageBase.hpp"

namespace membrane {
    // clang-format off
    //Types
    public ref struct Component{
        ComponentType type;
        System::Object ^initData;
    };
    public ref struct Entity{
        System::UInt32 id;
        System::String ^name;
        System::Collections::Generic::List<Component^> ^components;
    };

    //Messages sent from Bulb
    public ref class Editor_CreateEntity : public EditorMessage {};
    public ref class Editor_DeleteEntity : public EditorMessage {
    public:
        System::UInt32 entity;
    };
    public ref class Editor_AddComponent : public EditorMessage {
    public:
        System::UInt32 entity;
        ComponentType componentType;
    };
    public ref class Editor_RemoveComponent : public EditorMessage {
    public:
        System::UInt32 entity;
        ComponentType componentType;
    };
    public ref class Editor_UpdateTransform : public EditorMessage {
    public:
        System::UInt32 entity;

        Vector3 position;
        Vector3 rotation;
        Vector3 scale;
    };
    public ref class Editor_UpdateStaticModel : public EditorMessage {
    public:
        System::UInt32 entity;

        System::String ^meshPath;
        System::String ^diffusePath;
        System::String ^specularPath;
    };
    public ref class Editor_UpdateRigidBody : public EditorMessage {
    public:
        System::UInt32 entity;

        float mass{};
    };
    public ref class Editor_UpdateSphereShape : public EditorMessage {
    public:
        System::UInt32 entity;

        float radius{};
    };
    public ref class Editor_UpdateCubeShape : public EditorMessage {
    public:
        System::UInt32 entity;

        Vector3 halfExtents{};
    };
    public ref class Editor_UpdateName : public EditorMessage {
    public:
        System::UInt32 entity;

        System::String^ name;
    };
    public ref class Editor_ViewportKeyEvent : public EditorMessage {
    public:
        enum class Type{
            Pressed,
            Released,
        };

    public:
        System::Windows::Input::Key key;
        Type type;
    };
    public ref class Editor_ViewportMouseButtonEvent : public EditorMessage {
    public:
        System::Windows::Input::MouseButton button;
        System::Windows::Input::MouseButtonState state;
        System::Windows::Point position;
    };
    public ref class Editor_ViewportMouseMoveEvent : public EditorMessage {
    public:
        System::Windows::Point position;
    };
    public ref class Editor_SaveScene : public EditorMessage {};
    public ref class Editor_LoadScene : public EditorMessage {};
    public ref class Editor_Play : public EditorMessage {};
    public ref class Editor_Stop : public EditorMessage {};

    //Messages sent from Clove
    public ref class Engine_OnEntityCreated : public EngineMessage {
    public:
        System::UInt32 entity;
        System::String ^name;
    };
    public ref class Engine_OnEntityDeleted : public EngineMessage {
    public:
        System::UInt32 entity;
    };
    public ref class Engine_OnComponentAdded : public EngineMessage {
    public:
        System::UInt32 entity;
        ComponentType componentType;
        System::Object ^data;
    };
    public ref class Engine_OnComponentRemoved : public EngineMessage {
    public:
        System::UInt32 entity;
        ComponentType componentType;
    };
    public ref class Engine_OnTransformChanged : public EngineMessage {
    public:
        System::UInt32 entity;
        
        Vector3 position;
        Vector3 rotation;
        Vector3 scale;
    };
    public ref class Engine_OnRigidBodyChanged : public EngineMessage {
    public:
        System::UInt32 entity;

        float mass{};
    };
    public ref class Engine_OnSphereShapeChanged : public EngineMessage {
    public:
        System::UInt32 entity;

        float radius{};
    };
    public ref class Engine_OnCubeShapeChanged : public EngineMessage {
    public:
        System::UInt32 entity;

        Vector3 halfExtents{};
    };
    public ref class Engine_OnSceneLoaded : public EngineMessage {
    public:
        System::Collections::Generic::List<Entity^> ^entities;
    };
    // clang-format on
}