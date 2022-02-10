namespace clove {
    Camera::Camera() = default;

    Camera::Camera(Camera &&other) noexcept = default;

    Camera &Camera::operator=(Camera &&other) noexcept = default;

    Camera::~Camera() = default;

    void Camera::setProjectionMode(ProjectionMode const mode) {
        currentProjectionMode = mode;
    }

    void Camera::setZoomLevel(float zoom) {
        zoomLevel = zoom;
    }

    void Camera::setViewport(Viewport viewport) {
        this->viewport = viewport;
    }

    Camera::Viewport const &Camera::getViewport() const {
        return viewport;
    }
}