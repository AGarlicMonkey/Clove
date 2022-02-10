#include "Clove/Components/PointLightComponent.hpp"

#include "Clove/ReflectionAttributes.hpp"

#include <Clove/ECS/EntityManager.hpp>
#include <Clove/ECS/Entity.hpp>

CLOVE_REFLECT_BEGIN(clove::PointLightComponent, clove::EditorVisibleComponent{
                                                    .name                     = "Point Light Component",
                                                    .onEditorCreateComponent  = &createComponentHelper<clove::PointLightComponent>,
                                                    .onEditorGetComponent     = &getComponentHelper<clove::PointLightComponent>,
                                                    .onEditorDestroyComponent = &destroyComponentHelper<clove::PointLightComponent>,
                                                })
CLOVE_REFLECT_MEMBER(ambientColour, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(diffuseColour, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(specularColour, clove::EditorEditableMember{})
CLOVE_REFLECT_END