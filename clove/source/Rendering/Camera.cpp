#include "Clove/Rendering/Camera.hpp"

#include "Clove/Application.hpp"
#include "Clove/Surface.hpp"

#include <Clove/Maths/MathsHelpers.hpp>

namespace clove {
    Camera::Camera(Viewport viewport, ProjectionMode const projection)
        : viewport{ viewport }
        , currentProjectionMode{ projection } {
        setProjectionMode(projection);
    }

    Camera::Camera(ProjectionMode const projection)
        : currentProjectionMode{ projection } {
        auto *window{ Application::get().getSurface() };

        surfaceResizeHandle = window->onSurfaceResize().bind([this](vec2ui const &size) {
            setViewport({ 0, 0, static_cast<int32_t>(size.x), static_cast<int32_t>(size.y) });
        });

        viewport = { 0, 0, window->getSize().x, window->getSize().y };
        setProjectionMode(projection);
    }

    Camera::Camera(Camera &&other) noexcept
        : currentProjectionMode{ other.currentProjectionMode }
        , view{ other.view }
        , projection{ other.projection }
        , viewport{ other.viewport }
        , zoomLevel{ other.zoomLevel } {
        if(other.surfaceResizeHandle.isValid()) {
            surfaceResizeHandle = Application::get().getSurface()->onSurfaceResize().bind([this](vec2ui const &size) {
                setViewport({ 0, 0, static_cast<int32_t>(size.x), static_cast<int32_t>(size.y) });
            });
        }
    }

    Camera &Camera::operator=(Camera &&other) noexcept {
        currentProjectionMode = other.currentProjectionMode;
        view                  = other.view;
        projection            = other.projection;
        viewport              = other.viewport;
        zoomLevel             = other.zoomLevel;
        if(other.surfaceResizeHandle.isValid()) {
            surfaceResizeHandle = Application::get().getSurface()->onSurfaceResize().bind([this](vec2ui const &size) {
                setViewport({ 0, 0, static_cast<int32_t>(size.x), static_cast<int32_t>(size.y) });
            });
        }

        return *this;
    }

    Camera::~Camera() = default;

    void Camera::setView(mat4f view) {
        this->view = view;
    }

    void Camera::setProjectionMode(ProjectionMode const mode) {
        float constexpr orthographicSize{ 15.0f };
        float constexpr fov{ 45.0f };
        float const othoZoom{ orthographicSize * zoomLevel };

        float const width{ static_cast<float>(viewport.width) };
        float const height{ static_cast<float>(viewport.height) };
        float const aspect{ height > 0.0f ? width / height : 0.0f };

        currentProjectionMode = mode;

        switch(currentProjectionMode) {
            case ProjectionMode::Orthographic:
                projection = createOrthographicMatrix(-othoZoom * aspect, othoZoom * aspect, -othoZoom, othoZoom, nearPlane, farPlane);
                break;

            case ProjectionMode::Perspective:
                projection = createPerspectiveMatrix(fov * zoomLevel, aspect, nearPlane, farPlane);
                break;

            default:
                break;
        }
    }

    void Camera::setZoomLevel(float zoom) {
        zoomLevel = zoom;
        setProjectionMode(currentProjectionMode);
    }

    void Camera::setViewport(Viewport viewport) {
        this->viewport = viewport;
        setProjectionMode(currentProjectionMode);
    }

    mat4f const &Camera::getView() const {
        return view;
    }

    mat4f const &Camera::getProjection() const {
        return projection;
    }

    Camera::ProjectionMode Camera::getProjectionMode() const {
        return currentProjectionMode;
    }

    Viewport const &Camera::getViewport() const {
        return viewport;
    }
}