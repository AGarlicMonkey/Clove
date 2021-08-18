#pragma once

namespace membrane {
    // clang-format off
    public enum class ComponentType {
        Transform,
        StaticModel,
        PointLight,
        RigidBody,
        CollisionShape
    };

    public enum class ShapeType {
        Sphere,
        Cube
    };

    public value struct Vector3 {
        float x;
        float y;
        float z;

        Vector3(float x, float y, float z)
            : x{ x }
            , y{ y }
            , z{ z } {
        }
    };

    public ref class StaticModelComponentInitData{
    public:
        System::String ^meshPath;
        System::String ^diffusePath;
        System::String ^specularPath;
    };

    public ref class RigidBodyComponentInitData{
    public:
        float mass;
    };

    public ref class CollisionShapeComponentInitData{
    public:
        ShapeType shapeType;

        float radius;
        Vector3 halfExtents;
    };
    // clang-format on
}