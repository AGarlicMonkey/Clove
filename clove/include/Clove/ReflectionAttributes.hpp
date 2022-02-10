#pragma once

#include <Clove/ECS/Entity.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Reflection/Reflection.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>

CLOVE_DECLARE_LOG_CATEGORY(CloveReflection)

namespace clove {
    inline std::string getFloatValue(uint8_t const *const memory, size_t offset, size_t size) {
        CLOVE_ASSERT(size == sizeof(float));

        float const value{ *reinterpret_cast<float const *const>(memory + offset) };
        return std::to_string(value);
    }

    inline void setFloatValue(uint8_t *const memory, size_t offset, size_t size, std::string_view value) {
        CLOVE_ASSERT(size == sizeof(float));

        try {
            float const floatValue{ std::stof(std::string{ value }) };
            std::memcpy(memory + offset, &floatValue, size);
        } catch(std::exception e) {
            CLOVE_LOG(CloveReflection, LogLevel::Error, "Could not convert value {0} to float", value);
        }
    }

    template<typename ComponentType>
    uint8_t *createComponentHelper(clove::Entity entity, clove::EntityManager &manager) {
        return reinterpret_cast<uint8_t *>(&manager.addComponent<ComponentType>(entity));
    }

    template<typename ComponentType>
    uint8_t *getComponentHelper(clove::Entity entity, clove::EntityManager &manager) {
        return reinterpret_cast<uint8_t *>(&manager.getComponent<ComponentType>(entity));
    }

    template<typename ComponentType>
    void destroyComponentHelper(clove::Entity entity, clove::EntityManager &manager) {
        manager.removeComponent<ComponentType>(entity);
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
     * @brief Allows a member of a type to be modified as a string in editor.
     */
    struct EditorEditableMember {
        std::optional<std::string> name{}; /**< Name of the member. If not set then will just use the type's name. */

        std::function<std::string(uint8_t const *const, size_t, size_t)> onEditorGetValue{ &getFloatValue };      /**< Called when the editor retrieves the member. Override for custom serialisation logic. */
        std::function<void(uint8_t *const, size_t, size_t, std::string_view)> onEditorSetValue{ &setFloatValue }; /**< Called when the editor writes to the member. Override for custom serialisation logic. */
    };

    /**
     * @brief Allows a member of a type to be modified as a drop down in editor.
     */
    struct EditorEditableDropdown {
        std::optional<std::string> name{}; /**< Name of the member. If not set then will just use the type's name. */

        std::function<std::vector<std::string>()> getDropdownMembers{};                          /**< Returns all possible members for the drop down. */
        std::function<void(uint8_t *const, size_t, size_t, std::string_view)> setSelectedItem{}; /**< Sets the current dropdown selection. */
        std::function<size_t(uint8_t const *const, size_t, size_t)> getSelectedIndex{};          /**< Returns the index of the currently selected item. */

        std::function<reflection::TypeInfo const *(std::string_view)> getTypeInfoForMember{ nullptr }; /**< OPTIONAL. If set then when a member is selected this type info will also display.*/
    };
}