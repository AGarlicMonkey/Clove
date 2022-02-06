#include "Clove/Components/CameraComponent.hpp"

#include "Clove/ReflectionAttributes.hpp"

CLOVE_REFLECT_BEGIN(clove::CameraComponent, clove::EditorVisibleComponent{
                                                .name                     = "Camera Component",
                                                .onEditorCreateComponent  = &createComponentHelper<clove::CameraComponent>,
                                                .onEditorGetComponent     = &getComponentHelper<clove::CameraComponent>,
                                                .onEditorDestroyComponent = &destroyComponentHelper<clove::CameraComponent>,
                                            })
CLOVE_REFLECT_MEMBER(camera, clove::EditorEditableMember{})
CLOVE_REFLECT_END