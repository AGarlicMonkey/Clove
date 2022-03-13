#pragma once

#include "Concepts.hpp"

//Quaternion types
namespace clove {
    template<number T>
    struct quat {
        T x{ 0 };
        T y{ 0 };
        T z{ 0 };
        T w{ 1 };
    };
}

//Quaternion aliases
namespace clove {
    using quatf = quat<float>;
}

//Quaternion operations
namespace clove {

}

#include "Quaternion.inl"