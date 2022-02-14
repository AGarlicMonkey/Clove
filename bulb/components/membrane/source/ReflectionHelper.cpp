#include "Membrane/ReflectionHelper.hpp"

#include <Clove/Reflection/Reflection.hpp>
#include <Clove/ReflectionAttributes.hpp>
#include <msclr/marshal_cppstd.h>

using namespace clove;

namespace membrane {
    System::Collections::Generic::List<AvailableTypeInfo ^> ^ReflectionHelper::getEditorVisibleComponents() {
        System::Collections::Generic::List<AvailableTypeInfo ^> ^list { gcnew System::Collections::Generic::List<AvailableTypeInfo ^>{} };

        std::vector<reflection::TypeInfo const *> editorTypes{ reflection::getTypesWithAttribute<EditorVisibleComponent>() };
        for(auto const *typeInfo : editorTypes) {
            auto const visibleAttribute{ typeInfo->attributes.get<EditorVisibleComponent>().value() };

            AvailableTypeInfo ^availableTypeInfo { gcnew AvailableTypeInfo{} };
            availableTypeInfo->displayName = gcnew System::String{ visibleAttribute.name.value_or(typeInfo->name).c_str() };
            availableTypeInfo->typeName    = gcnew System::String{ typeInfo->name.c_str() };
            list->Add(availableTypeInfo);
        }

        return list;
    }

    System::Collections::Generic::List<AvailableTypeInfo ^> ^ ReflectionHelper::getEditorVisibleSubSystems() {
        System::Collections::Generic::List<AvailableTypeInfo ^> ^ list { gcnew System::Collections::Generic::List<AvailableTypeInfo ^>{} };

        std::vector<reflection::TypeInfo const *> editorTypes{ reflection::getTypesWithAttribute<EditorVisibleSubSystem>() };
        for(auto const *typeInfo : editorTypes) {
            auto const visibleAttribute{ typeInfo->attributes.get<EditorVisibleSubSystem>().value() };

            AvailableTypeInfo ^ availableTypeInfo { gcnew AvailableTypeInfo{} };
            availableTypeInfo->displayName = gcnew System::String{ visibleAttribute.name.value_or(typeInfo->name).c_str() };
            availableTypeInfo->typeName    = gcnew System::String{ typeInfo->name.c_str() };
            list->Add(availableTypeInfo);
        }

        return list;
    }
}