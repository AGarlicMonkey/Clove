#pragma once

/**
 * Contains serisation overloads for types included from other libraries
 */

#include <Clove/Maths/Vector.hpp>
#include <Clove/Maths/Quaternion.hpp>
#include <Clove/Serialisation/Node.hpp>

namespace clove {
    //Vec3
    template<>
    inline serialiser::Node serialise(vec3f const &object) {
        serialiser::Node node{};
        node.pushBack(object.x);
        node.pushBack(object.y);
        node.pushBack(object.z);
        return node;
    }
    template<>
    inline vec3f deserialise(serialiser::Node const &node) {
        vec3f v{};
        for(size_t i{ 0 }; i < node.numChildren(); ++i) {
            v[i] = node[i].as<float>();
        }
        return v;
    }

    //Vec4
    template<>
    inline serialiser::Node serialise(vec4f const &object) {
        serialiser::Node node{};
        node.pushBack(object.x);
        node.pushBack(object.y);
        node.pushBack(object.z);
        node.pushBack(object.w);
        return node;
    }
    template<>
    inline vec4f deserialise(serialiser::Node const &node) {
        vec4f v{};
        for(size_t i{ 0 }; i < node.numChildren(); ++i) {
            v[i] = node[i].as<float>();
        }
        return v;
    }

    //Quat
    template<>
    inline serialiser::Node serialise(quatf const &object) {
        serialiser::Node node{};
        node.pushBack(object.x);
        node.pushBack(object.y);
        node.pushBack(object.z);
        node.pushBack(object.w);
        return node;
    }
    template<>
    inline quatf deserialise(serialiser::Node const &node) {
        quatf q{};
        for(size_t i{ 0 }; i < node.numChildren(); ++i) {
            q[i] = node[i].as<float>();
        }
        return q;
    }
}