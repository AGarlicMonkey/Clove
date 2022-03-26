#pragma once

#include "Membrane/Export.hpp"

#include <Clove/ECS/Entity.hpp>

MEMBRANE_EXPORT clove::Entity createEntity();
MEMBRANE_EXPORT void deleteEntity(clove::Entity entity);