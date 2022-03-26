#include "Membrane/EntityComponentSystem.hpp"

#include <Clove/Application.hpp>
#include <Clove/ECS/EntityManager.hpp>
#include <codecvt>
#include <Clove/Reflection/Reflection.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/ReflectionAttributes.hpp>

CLOVE_DECLARE_LOG_CATEGORY(MembraneECS)

using namespace clove;

Entity createEntity() {
    return Application::get().getEntityManager()->create();
}

void deleteEntity(clove::Entity entity) {
    Application::get().getEntityManager()->destroy(entity);
}

void addComponent(clove::Entity entity, wchar_t const *componentTypeName) {
    std::string const narrowComponentName{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(componentTypeName) };

    reflection::TypeInfo const *componentTypeInfo{ reflection::getTypeInfo(narrowComponentName) };
    if(componentTypeInfo == nullptr) {
        CLOVE_LOG(MembraneECS, LogLevel::Error, "Component with name {0} does not exist. Adding component has failed.");
        return;
    }

    auto editorAttribute{ componentTypeInfo->attributes.get<EditorVisibleComponent>().value() };
    editorAttribute.onEditorCreateComponent(entity, *Application::get().getEntityManager());

    //TODO: return type info
}