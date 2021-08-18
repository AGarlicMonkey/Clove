#pragma once

#include "Clove/Rendering/Renderables/Texture.hpp"

#include <Clove/Expected.hpp>
#include <filesystem>

namespace clove::TextureLoader {
    /**
     * @brief Loads a texture. Returning the result or the reason for failure.
     * @param path Path to texture.
     * @return 
     */
    Expected<Texture, std::runtime_error> loadTexture(std::filesystem::path const &path);
}