#include "Clove/Rendering/Camera.hpp"

#include "Clove/ReflectionAttributes.hpp"
#include "Clove/Rendering/RenderingLog.hpp"

#include <Clove/Maths/MathsHelpers.hpp>

namespace clove {
    Camera::Camera(ProjectionMode const projection)
        : currentProjectionMode{ projection } {
    }

    Camera::Camera(ProjectionMode const projection, Viewport const viewport)
        : currentProjectionMode{ projection }
        , viewport{ viewport } {
    }

    mat4f Camera::getProjection(vec2ui const screenSize) const {
        float constexpr orthographicSize{ 15.0f };
        float constexpr fov{ 45.0f };
        float const othoZoom{ orthographicSize * zoomLevel };

        float const width{ static_cast<float>(screenSize.x) * viewport.width };
        float const height{ static_cast<float>(screenSize.y) * viewport.height };
        float const aspect{ height > 0.0f ? width / height : 0.0f };

        switch(currentProjectionMode) {
            case ProjectionMode::Orthographic:
                return createOrthographicMatrix(-othoZoom * aspect, othoZoom * aspect, -othoZoom, othoZoom, nearPlane, farPlane);
            case ProjectionMode::Perspective:
                return createPerspectiveMatrix(fov * zoomLevel, aspect, nearPlane, farPlane);
            default:
                CLOVE_ASSERT_MSG(false, "{0}: Case not handled", CLOVE_FUNCTION_NAME_PRETTY);
                return mat4f{ 1.0f };
        }
    }
}

CLOVE_REFLECT_BEGIN(clove::Camera)
CLOVE_REFLECT_MEMBER(currentProjectionMode, clove::EditorEditableDropdown{
                                                .name               = "Projection Mode",
                                                .getDropdownMembers = []() -> std::vector<std::string> {
                                                    return { "Orthographic", "Perspective" };
                                                },
                                                .setSelectedItem  = [](uint8_t *const memory, size_t offset, size_t size, std::string_view selection) {
                                                    CLOVE_ASSERT(size == sizeof(clove::Camera::ProjectionMode));
                                                    auto *const mode{ reinterpret_cast<clove::Camera::ProjectionMode *const>(memory) };

                                                    if(selection == "Orthographic"){
                                                        *mode = clove::Camera::ProjectionMode::Orthographic;
                                                    } else if(selection == "Perspective") {
                                                        *mode = clove::Camera::ProjectionMode::Perspective;
                                                    } 
                                                },
                                                .getSelectedIndex = [](uint8_t const *const memory, size_t offset, size_t size) -> size_t {
                                                    CLOVE_ASSERT(size == sizeof(clove::Camera::ProjectionMode));
                                                    auto *const mode{ reinterpret_cast<clove::Camera::ProjectionMode const *const>(memory) };

                                                    switch(*mode) {
                                                        case clove::Camera::ProjectionMode::Orthographic:
                                                            return 0;
                                                        case clove::Camera::ProjectionMode::Perspective:
                                                        default:
                                                            return 1;
                                                    }
                                                },
                                            })
CLOVE_REFLECT_END