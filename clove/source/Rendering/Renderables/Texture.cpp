#include "Clove/Rendering/Renderables/Texture.hpp"

#include "Clove/Application.hpp"
#include "Clove/Rendering/RenderingHelpers.hpp"

#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Graphics/GhaImageView.hpp>

namespace clove {
    Texture::Texture(std::unique_ptr<void, std::function<void(void *)>> buffer, vec2ui dimensions, int32_t channels)
        : buffer{ std::move(buffer) }
        , dimensions{ dimensions }
        , channels{ channels } {
        GhaFactory &factory{ *Application::get().getGraphicsDevice()->getGraphicsFactory() };

        GhaImage::Descriptor const textureDesc{
            .type        = GhaImage::Type::_2D,
            .usageFlags  = GhaImage::UsageMode::Sampled,
            .dimensions  = this->dimensions,
            .format      = GhaImage::Format::R8G8B8A8_SRGB,
            .sharingMode = SharingMode::Exclusive,
        };

        size_t const size{ this->dimensions.x * this->dimensions.y * this->channels };

        image     = createImageWithData(factory, textureDesc, this->buffer.get(), size);
        imageView = image->createView(GhaImageView::Descriptor{
            .type       = GhaImageView::Type::_2D,
            .layer      = 0,
            .layerCount = 1,
        });
    }

    Texture::Texture(Texture &&other) noexcept = default;

    Texture &Texture::operator=(Texture &&other) noexcept = default;

    Texture::~Texture() = default;
}