#include "Clove/ReflectionCommon.hpp"

#include "Clove/ReflectionAttributes.hpp"

CLOVE_REFLECT_BEGIN(clove::vec3f)
CLOVE_REFLECT_MEMBER(x, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(y, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(z, clove::EditorEditableMember{})
CLOVE_REFLECT_END

CLOVE_REFLECT_BEGIN(clove::quatf)
CLOVE_REFLECT_MEMBER(x, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(y, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(z, clove::EditorEditableMember{})
CLOVE_REFLECT_MEMBER(w, clove::EditorEditableMember{})
CLOVE_REFLECT_END
