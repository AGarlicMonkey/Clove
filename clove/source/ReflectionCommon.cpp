#include "Clove/ReflectionAttributes.hpp"

#include <Clove/Maths/Quaternion.hpp>
#include <Clove/Maths/Vector.hpp>
#include <Clove/Reflection/Reflection.hpp>

//Provides reflection information for common types

CLOVE_REFLECT_DECLARE_TYPE(clove::vec3f)

CLOVE_REFLECT_BEGIN(clove::vec3f)
CLOVE_REFLECT_MEMBER(x, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(y, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(z, clove::EditorEditableMember{})
CLOVE_REFLECT_END

CLOVE_REFLECT_DECLARE_TYPE(clove::quatf)

CLOVE_REFLECT_BEGIN(clove::quatf)
CLOVE_REFLECT_MEMBER(x, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(y, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(z, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(w, clove::EditorEditableMember{})
CLOVE_REFLECT_END
