#pragma once

#include "Membrane/Export.hpp"

#include <Clove/Reflection/Reflection.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <string>
#include <wtypes.h>

struct AvailableEditorTypeInfo {
    BSTR typeName{ nullptr };
    BSTR displayName{ nullptr };
};

enum class TypeInfoType {
    Value,
    Dropdown,
    Parent,
};

struct EditorMemberInfo {
    BSTR name{ nullptr };

    uint64_t offset;//TODO: Use size_t - need 64 to make sure it's the same size as managed
    uint64_t size;  //TODO: Use size_t - need 64 to make sure it's the same size as managed
    uint64_t typeId;//TODO: Use size_t - need 64 to make sure it's the same size as managed
};

struct EditorTypeInfo {
    BSTR typeName{ nullptr };
    BSTR displayName{ nullptr };

    uint64_t typeId;//TODO: Use size_t - need 64 to make sure it's the same size as managed
    uint64_t size;//TODO: Use size_t - need 64 to make sure it's the same size as managed
    uint8_t *typeMemory{ nullptr };
};

/**
 * @brief This function has two ways to be called. If outInfos is nullptr then it will populate
 * numInfos with how many components are visible. If outInfos is not nullptr then it will
 * populate it as an array with the amount of numInfos.
 * @param outInfos Where to place the EditorTypeInfos for each visible component. Can be nullptr (see brief).
 * @param numInfos Will either return the amount of visible components or decide how many to return (See brief).
 * @return 
 */
MEMBRANE_EXPORT void getEditorVisibleComponents(AvailableEditorTypeInfo outInfos[], uint32_t &numInfos);
/**
 * @brief This function has two ways to be called. If outInfos is nullptr then it will populate
 * numInfos with how many components are visible. If outInfos is not nullptr then it will
 * populate it as an array with the amount of numInfos.
 * @param outInfos Where to place the EditorTypeInfos for each visible subsystem. Can be nullptr (see brief).
 * @param numInfos Will either return the amount of visible subsystems or decide how many to return (See brief).
 * @return 
 */
MEMBRANE_EXPORT void getEditorVisibleSubSystems(AvailableEditorTypeInfo outInfos[], uint32_t &numInfos);

MEMBRANE_EXPORT bool isTypeIdReflected(uint64_t const typeId);
MEMBRANE_EXPORT bool isMemberADropdown(uint64_t const parentTypeId, uint64_t const memberOffset);

MEMBRANE_EXPORT int32_t getMemberCountWithTypeName(wchar_t const *typeName);
MEMBRANE_EXPORT int32_t getMemberCountWithTypeId(uint64_t typeId);

MEMBRANE_EXPORT void getTypeInfoFromTypeId(uint64_t typeId, EditorTypeInfo &outTypeInfo, EditorMemberInfo outMembers[]);

MEMBRANE_EXPORT BSTR retrieveMemberValue(void *memberMemory, uint64_t const memberParentTypeId, uint64_t const totalOffsetIntoMemory, uint64_t const memberOffsetFromParentType, uint64_t const memberSize);

namespace membrane {
    void constructComponentEditorTypeInfo(clove::reflection::TypeInfo const *componentTypeInfo, EditorTypeInfo &outEditorComponentTypeInfo, EditorMemberInfo outComponentMembers[]);
}