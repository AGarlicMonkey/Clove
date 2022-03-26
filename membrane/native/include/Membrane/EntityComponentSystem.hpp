#pragma once

#include "Membrane/Export.hpp"

#include <Clove/ECS/Entity.hpp>

MEMBRANE_EXPORT clove::Entity createEntity();
MEMBRANE_EXPORT void deleteEntity(clove::Entity entity);

MEMBRANE_EXPORT void addComponent(clove::Entity entity, wchar_t const *componentTypeName);