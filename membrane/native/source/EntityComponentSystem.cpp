#include "Membrane/EntityComponentSystem.hpp"

#include <Clove/Application.hpp>
#include <Clove/ECS/EntityManager.hpp>

using namespace clove;

Entity createEntity() {
    return Application::get().getEntityManager()->create();
}

void deleteEntity(clove::Entity entity) {
    Application::get().getEntityManager()->destroy(entity);
}