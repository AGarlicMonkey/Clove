#include "Clove/ECS/ComponentContainer.hpp"

namespace clove{
    ComponentContainerInterface::ComponentContainerInterface() = default;

    ComponentContainerInterface::ComponentContainerInterface(ComponentContainerInterface &&other) noexcept = default;

    ComponentContainerInterface &ComponentContainerInterface::operator=(ComponentContainerInterface &&other) noexcept = default;

    ComponentContainerInterface::~ComponentContainerInterface() = default;
}