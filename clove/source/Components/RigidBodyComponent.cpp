#include "Clove/Components/RigidBodyComponent.hpp"

#include "Clove/ReflectionAttributes.hpp"

#include <Clove/ECS/Entity.hpp>
#include <Clove/ECS/EntityManager.hpp>

namespace clove {
    void RigidBodyComponent::applyForce(vec3f force, vec3f relativeOffset) {
        appliedForce = ForceApplication{ .amount = force, .offset = relativeOffset };
    }

    void RigidBodyComponent::applyImpulse(vec3f impulse, vec3f relativeOffset) {
        appliedImpulse = ForceApplication{ .amount = impulse, .offset = relativeOffset };
    }
}

CLOVE_REFLECT_BEGIN(clove::RigidBodyComponent, clove::EditorVisibleComponent{
                                                   .name                     = "Rigid Body Component",
                                                   .onEditorCreateComponent  = &createComponentHelper<clove::RigidBodyComponent>,
                                                   .onEditorGetComponent     = &getComponentHelper<clove::RigidBodyComponent>,
                                                   .onEditorDestroyComponent = &destroyComponentHelper<clove::RigidBodyComponent>,
                                               })
CLOVE_REFLECT_MEMBER(collisionGroup, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(collisionMask, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(mass, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(restitution, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(angularFactor, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(linearFactor, clove::EditorEditableMember{})
CLOVE_REFLECT_END