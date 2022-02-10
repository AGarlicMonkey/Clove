#pragma once

#include "Membrane/MessageBase.hpp"

namespace membrane {
    ref class EditorTypeInfo;
}

namespace membrane {
    // clang-format off
    //Types
    public ref struct Entity{
        System::UInt32 id;
        System::String ^name;
        System::Collections::Generic::List<EditorTypeInfo^> ^components;
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
        System::String ^componentName;
    };
    public ref class Editor_ModifyComponent : public EditorMessage {
    public:
        System::UInt32 entity;
        System::String ^componentName;
        System::UInt32 offset;
        System::String ^value;
    };
    public ref class Editor_RemoveComponent : public EditorMessage {
    public:
        System::UInt32 entity;
        System::String ^componentName;
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
        System::String ^componentName;
        EditorTypeInfo ^typeInfo;
    };
    public ref class Engine_OnComponentModified : public EngineMessage {
    public:
        System::UInt32 entity;
        System::String ^componentName;
        EditorTypeInfo ^typeInfo;
    };
    public ref class Engine_OnComponentRemoved : public EngineMessage {
    public:
        System::UInt32 entity;
        System::String ^componentName;
    };

    public ref class Engine_OnSceneLoaded : public EngineMessage {
    public:
        System::Collections::Generic::List<Entity^> ^entities;
    };
    // clang-format on
}