#pragma once

#include <Clove/Delegate/DelegateHandle.hpp>
#include <Clove/Maths/Matrix.hpp>

namespace clove {
    class Window;

    struct Viewport {
        int32_t x{ 0 };
        int32_t y{ 0 };
        int32_t width{ 0 };
        int32_t height{ 0 };
    };
}

namespace clove {
    class Camera {
        //TYPES
    public:
        enum class ProjectionMode {
            Orthographic,
            Perspective
        };

        //VARIABLES
    private:
        static float constexpr nearPlane{ 0.5f };
        static float constexpr farPlane{ 10000.0f };

        ProjectionMode currentProjectionMode;
        mat4f view{ 1.0f };
        mat4f projection{ 1.0f };

        Viewport viewport;

        float zoomLevel{ 1.0f };

        DelegateHandle surfaceResizeHandle;

        //FUNCTIONS
    public:
        Camera() = delete;
        Camera(Viewport viewport, ProjectionMode const projection);
        Camera(ProjectionMode const projection);

        Camera(Camera const &other) = delete;
        Camera(Camera &&other) noexcept;

        Camera &operator=(Camera const &other) = delete;
        Camera &operator=(Camera &&other) noexcept;

        ~Camera();

        inline float getNearPlane() const;
        inline float getFarPlane() const;

        void setView(mat4f view);
        void setProjectionMode(ProjectionMode const mode);

        void setZoomLevel(float zoom);

        void setViewport(Viewport viewport);

        mat4f const &getView() const;
        mat4f const &getProjection() const;

        ProjectionMode getProjectionMode() const;

        Viewport const &getViewport() const;
    };
}

#include "Camera.inl"