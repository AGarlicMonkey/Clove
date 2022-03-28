#include "Membrane/EntityComponentSystem.hpp"

#include <Clove/Application.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Reflection/Reflection.hpp>
#include <Clove/ReflectionAttributes.hpp>
#include <codecvt>

CLOVE_DECLARE_LOG_CATEGORY(MembraneECS)

using namespace clove;

Entity createEntity() {
    return Application::get().getEntityManager()->create();
}

void deleteEntity(clove::Entity entity) {
    Application::get().getEntityManager()->destroy(entity);
}

bool addComponent(clove::Entity entity, wchar_t const *componentTypeName, EditorTypeInfo &outTypeInfo) {
    std::string const narrowComponentName{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(componentTypeName) };
    reflection::TypeInfo const *const componentTypeInfo{ reflection::getTypeInfo(narrowComponentName) };

    if(componentTypeInfo == nullptr) {
        CLOVE_LOG(MembraneECS, LogLevel::Error, "Component with name {0} does not exist. Adding component has failed.");
        return false;
    }

    auto const editorAttribute{ componentTypeInfo->attributes.get<EditorVisibleComponent>().value() };
    uint8_t const *const componentMemory{ editorAttribute.onEditorCreateComponent(entity, *Application::get().getEntityManager()) };

    try {
        membrane::constructComponentEditorTypeInfo(componentTypeInfo, componentMemory, outTypeInfo);
        return true;
    } catch(std::exception e) {
        CLOVE_LOG(MembraneECS, LogLevel::Error, "Failed to construct editor type info for component {0}:", componentTypeInfo->name);
        CLOVE_LOG(MembraneECS, LogLevel::Error, "\t{0}", e.what());
        CLOVE_LOG(MembraneECS, LogLevel::Error, "Component has been added to entity but will display incorrectly in editor.");
        return false;
    }
}