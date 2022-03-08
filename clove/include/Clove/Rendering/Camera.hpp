#pragma once

#include <Clove/Delegate/DelegateHandle.hpp>
#include <Clove/Maths/Matrix.hpp>
#include <Clove/Maths/Vector.hpp>
#include <Clove/Reflection/Reflection.hpp>

namespace clove {
    class Camera {
        CLOVE_REFLECT_PRIVATE(Camera)

        //TYPES
    public:
        /**
         * @brief Describes a region within the screen that this camera renders to.
         */
        struct Viewport {
            float x{ 0.0f };      /**< 0 - 1 value for the start of the horizontal value. */
            float y{ 0.0f };      /**< 0 - 1 value for the start of the vertical value. */
            float width{ 1.0f };  /**< 0 - 1 value for the end of the horizontal value. */
            float height{ 1.0f }; /**< 0 - 1 value for the end of the vertical value. */
        };

        enum class ProjectionMode {
            Orthographic,
            Perspective
        };

        //VARIABLES
    private:
        static float constexpr nearPlane{ 0.5f };
        static float constexpr farPlane{ 10000.0f };

        Viewport viewport{};
        ProjectionMode currentProjectionMode{ ProjectionMode::Perspective };

        float zoomLevel{ 1.0f };

        //FUNCTIONS
    public:
        inline Camera();
        Camera(ProjectionMode const projection);
        Camera(ProjectionMode const projection, Viewport const viewport);

        Camera(Camera const &other) = delete;
        inline Camera(Camera &&other) noexcept;

        Camera &operator=(Camera const &other) = delete;
        inline Camera &operator=(Camera &&other) noexcept;

        inline ~Camera();

        inline void setProjectionMode(ProjectionMode const mode);
        inline void setZoomLevel(float zoom);

        mat4f getProjection(vec2ui const screenSize) const;

        inline void setViewport(Viewport viewport);
        inline Viewport const &getViewport() const;
    };
}

#include "Camera.inl"

CLOVE_REFLECT_DECLARE_TYPE(clove::Camera)