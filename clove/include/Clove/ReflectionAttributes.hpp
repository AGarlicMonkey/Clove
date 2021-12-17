#pragma once

#include <Clove/ECS/Entity.hpp>
#include <Clove/Reflection/Reflection.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveReflection)

namespace clove {
    class EntityManager;
}

namespace clove {
    inline std::string defaultEditorGetValue(uint8_t const *const memory, size_t offset, size_t size) {
        //NOTE: Currently assuming everything using this is a float
        CLOVE_ASSERT(size == sizeof(float));

        float const value{ *reinterpret_cast<float const *const>(memory + offset) };
        return std::to_string(value);
    }

    inline void defaultEditorSetValue(uint8_t *const memory, size_t offset, size_t size, std::string_view value) {
        //NOTE: Currently assuming everything using this is a float
        CLOVE_ASSERT(size == sizeof(float));
        
        try {
            float const floatValue{ std::stof(std::string{ value }) };
            std::memcpy(memory + offset, &floatValue, size);
        } catch(std::exception e) {
            CLOVE_LOG(CloveReflection, LogLevel::Error, "Could not convert value {0} to float", value);
        }
    }
}

namespace clove {
    /**
     * @brief Signifies that this type wants to be displayed in editor as an entity component.
     */
    struct EditorVisibleComponent {
        std::optional<std::string> name{}; /**< Name of the class. If not set then will just use the type's name. */

        std::function<uint8_t *(Entity, EntityManager &)> onEditorCreateComponent{}; /**< Called when the editor wants to create this component. */
        std::function<uint8_t *(Entity, EntityManager &)> onEditorGetComponent{};    /**< Called when the editor wants to get access to this component. */
        std::function<void(Entity, EntityManager &)> onEditorDestroyComponent{};     /**< Called when the editor wants to destroy this component.*/
    };

    /**
     * @brief Allows a member of a type to be modified in editor.
     */
    struct EditorEditableMember {
        std::optional<std::string> name{}; /**< Name of the member. If not set then will just use the type's name. */

        std::function<std::string(uint8_t const *const, size_t, size_t)> onEditorGetValue{ &defaultEditorGetValue };      /**< Called when the editor retrieves the member. Override for custom serialisation logic. */
        std::function<void(uint8_t *const, size_t, size_t, std::string_view)> onEditorSetValue{ &defaultEditorSetValue }; /**< Called when the editor writes to the member. Override for custom serialisation logic. */
    };
}