#include "Membrane/Reflection.hpp"

#include <Clove/Log/Log.hpp>
#include <Clove/ReflectionAttributes.hpp>
#include <codecvt>
#include <vector>

CLOVE_DECLARE_LOG_CATEGORY(MembraneReflection)

using namespace clove;

namespace {
    template<typename AttributeType>
    void getEditorVisibileTypeForAttribute(AvailableEditorTypeInfo outInfos[], uint32_t &numInfos) {
        std::vector<reflection::TypeInfo const *> visibleComponents{ reflection::getTypesWithAttribute<AttributeType>() };

        if(outInfos == nullptr) {
            numInfos = visibleComponents.size();
        } else {
            CLOVE_LOG(MembraneReflection, LogLevel::Trace, "{0} called with an array of {1} AvailableEditorTypeInfos as an output", CLOVE_FUNCTION_NAME, numInfos);

            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter{};

            for(size_t i{ 0 }; i < numInfos; ++i) {
                reflection::TypeInfo const *typeInfo{ visibleComponents[i] };
                auto const attribute{ typeInfo->attributes.get<AttributeType>().value() };

                outInfos[i] = AvailableEditorTypeInfo{
                    .typeName    = SysAllocString(stringConverter.from_bytes(typeInfo->name).c_str()),
                    .displayName = SysAllocString(stringConverter.from_bytes(attribute.name.value_or(typeInfo->name)).c_str()),
                };
            }
        }
    }
}

void getEditorVisibleComponents(AvailableEditorTypeInfo outInfos[], uint32_t &numInfos) {
    return getEditorVisibileTypeForAttribute<EditorVisibleComponent>(outInfos, numInfos);
}

void getEditorVisibleSubSystems(AvailableEditorTypeInfo outInfos[], uint32_t &numInfos) {
    return getEditorVisibileTypeForAttribute<EditorVisibleSubSystem>(outInfos, numInfos);
}

int32_t getMemberCountForType(wchar_t const *typeName) {
    std::string const narrowTypeName{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(typeName) };
    reflection::TypeInfo const *const typeInfo{ reflection::getTypeInfo(narrowTypeName) };

    return typeInfo != nullptr ? typeInfo->members.size() : 0;
}

namespace membrane {
    void constructComponentEditorTypeInfo(reflection::TypeInfo const *const componentTypeInfo, EditorTypeInfo &outEditorComponentTypeInfo, EditorMemberInfo outComponentMembers[]) {
        EditorVisibleComponent const attribute{ componentTypeInfo->attributes.get<EditorVisibleComponent>().value() };
        std::vector<reflection::MemberInfo> const &typeMembers{ componentTypeInfo->members };
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter{};

        outEditorComponentTypeInfo.typeName    = SysAllocString(stringConverter.from_bytes(componentTypeInfo->name).c_str());
        outEditorComponentTypeInfo.displayName = SysAllocString(stringConverter.from_bytes(attribute.name.value_or(componentTypeInfo->name)).c_str());
        outEditorComponentTypeInfo.size        = componentTypeInfo->size;

        for(size_t i{ 0 }; i < typeMembers.size(); ++i) {
            outComponentMembers[i].name   = SysAllocString(stringConverter.from_bytes(typeMembers[i].name).c_str());
            outComponentMembers[i].offset = typeMembers[i].offset;
            outComponentMembers[i].size   = typeMembers[i].size;
            //outComponentMembers[i].typeId = typeMembers[i].id;
        }
    }
}