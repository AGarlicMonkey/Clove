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
    uint32_t typeId;
};

struct EditorTypeInfo {
    BSTR typeName{ nullptr };
    BSTR displayName{ nullptr };

    EditorMemberInfo *members{ nullptr };
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

MEMBRANE_EXPORT int32_t getMemberCountForType(wchar_t const *typeName);

namespace membrane {
    void constructComponentEditorTypeInfo(clove::reflection::TypeInfo const *componentTypeInfo, void const *const componentMemory, EditorTypeInfo &outEditorComponentTypeInfo);
}