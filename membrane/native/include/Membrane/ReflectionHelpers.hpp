#pragma once

#include "Membrane/Export.hpp"

#include <Clove/Reflection/Reflection.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <string>

/*
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

        System::UInt32 offset{ 0 };

        EditorTypeType type{};
        System::Object ^typeData{}; //Data dependant on the type. If it is a value then it is a string, if it is a parent then it is an array of members etc.

        bool dragDropOnly{ false };
    };
    */

struct EditorTypeInfo {
    std::string typeName{};
    std::string displayName{};
};

/**
 * @brief This function has two ways to be called. If outInfos is nullptr then it will populate
 * numInfos with how many components are visible. If outInfos is not nullptr then it will
 * populate it as an array with the amount of numInfos.
 * @param outInfos Where to place the EditorTypeInfos for each visible component. Can be nullptr (see brief).
 * @param numInfos Will either return the amount of visible components or decide how many to return (See brief).
 * @return 
 */
MEMBRANE_EXPORT void getEditorVisibleComponents(EditorTypeInfo *outInfos, uint32_t &numInfos);
/**
 * @brief This function has two ways to be called. If outInfos is nullptr then it will populate
 * numInfos with how many components are visible. If outInfos is not nullptr then it will
 * populate it as an array with the amount of numInfos.
 * @param outInfos Where to place the EditorTypeInfos for each visible subsystem. Can be nullptr (see brief).
 * @param numInfos Will either return the amount of visible subsystems or decide how many to return (See brief).
 * @return 
 */
MEMBRANE_EXPORT void getEditorVisibleSubSystems(EditorTypeInfo *outInfos, uint32_t &numInfos);

namespace membrane {
    /*
    System::Collections::Generic::List<EditorTypeInfo ^> ^ constructMembers(std::vector<clove::reflection::MemberInfo> const &members, void const *const memory, size_t offsetIntoParent);
    EditorTypeInfo ^ constructComponentEditorTypeInfo(clove::reflection::TypeInfo const *typeInfo, void const *const memory);

    void modifyComponentMember(uint8_t *const memory, clove::reflection::TypeInfo const *typeInfo, std::string_view value, size_t const requiredOffset, size_t currentOffset);

    clove::serialiser::Node serialiseComponent(clove::reflection::TypeInfo const *const componentTypeInfo, uint8_t const *const componentMemory, size_t currentOffset = 0);
    void deserialiseComponent(clove::reflection::TypeInfo const *const componentTypeInfo, uint8_t *const componentMemory, clove::serialiser::Node const &componentNode, size_t currentOffset = 0);
    */
}