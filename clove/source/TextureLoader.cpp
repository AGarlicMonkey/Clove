#include "Clove/TextureLoader.hpp"

#include <stb_image.h>
#include <functional>

namespace clove::TextureLoader {
    Expected<Texture, std::runtime_error> loadTexture(std::filesystem::path const &path) {
        int width{ 0 };
        int height{ 0 };
        int channels{ 0 };
        void *localBuffer{ stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha) };

        if(localBuffer == nullptr) {
            return Unexpected{ std::runtime_error{ "Failed to load texture!" } };
        }

        vec2ui dimensions{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
        auto buffer{ std::unique_ptr<void, std::function<void(void *)>>(localBuffer, [](void *buffer) { stbi_image_free(buffer); }) };
        return Texture{ std::move(buffer), dimensions, STBI_rgb_alpha };
    }
}