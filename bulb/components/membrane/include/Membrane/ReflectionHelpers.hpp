#pragma once

#include <Clove/Serialisation/Node.hpp>
#include <Clove/Reflection/Reflection.hpp>

namespace membrane {
    public enum class EditorTypeType{
        Value,
        Parent,
        Dropdown,
    };

    public ref class EditorTypeDropdown {
        //VARIABLES
    public:
        System::Int32 currentSelection{};
        System::Collections::Generic::List<System::String ^> ^ dropdownItems { nullptr };

        System::Collections::Generic::List<ref class EditorTypeInfo ^> ^ dropdownTypeInfos { nullptr };
    };

    public ref class EditorTypeInfo {
        //VARIABLES
    public:
        System::String ^typeName { nullptr };
        System::String ^displayName { nullptr };

        System::UInt32 offset{ 0 }; /**< Offset into the parent of this type. */

        EditorTypeType type{};
        System::Object ^typeData{}; //Data dependant on the type. If it is a value then it is a string, if it is a parent then it is an array of members etc.
    };

    public ref class AvailableTypeInfo {
        //VARIABLES
    public:
        System::String ^typeName { nullptr };
        System::String ^displayName { nullptr };
    };

    /**
     * @brief Allows the editor to easily access reflected types.
     */
    public ref class ReflectionHelper {
        //FUNCTIONS
    public:
        static System::Collections::Generic::List<AvailableTypeInfo ^> ^getEditorVisibleComponents();

        static System::Collections::Generic::List<AvailableTypeInfo ^> ^getEditorVisibleSubSystems();
    };

    System::Collections::Generic::List<EditorTypeInfo ^> ^ constructMembers(std::vector<clove::reflection::MemberInfo> const &members, void const *const memory, size_t offsetIntoParent);
    EditorTypeInfo ^ constructComponentEditorTypeInfo(clove::reflection::TypeInfo const *typeInfo, void const *const memory);

    void modifyComponentMember(uint8_t *const memory, clove::reflection::TypeInfo const *typeInfo, std::string_view value, size_t const requiredOffset, size_t currentOffset);

    clove::serialiser::Node serialiseComponent(clove::reflection::TypeInfo const *const componentTypeInfo, uint8_t const *const componentMemory, size_t currentOffset = 0);
    void deserialiseComponent(clove::reflection::TypeInfo const *const componentTypeInfo, uint8_t *const componentMemory, clove::serialiser::Node const &componentNode, size_t currentOffset = 0);
}