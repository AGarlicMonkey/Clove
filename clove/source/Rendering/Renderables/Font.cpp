#include "Clove/Rendering/Renderables/Font.hpp"

#include "Clove/Application.hpp"
#include "Clove/Rendering/RenderingHelpers.hpp"

#include <Clove/Graphics/GhaDevice.hpp>
#include <Clove/Graphics/GhaFactory.hpp>
#include <Clove/Graphics/GhaImage.hpp>
#include <Clove/Log/Log.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Clove/Definitions.hpp>

extern "C" const unsigned char roboto_black[];
extern "C" const size_t roboto_blackLength;

CLOVE_DECLARE_LOG_CATEGORY(FreeType)
CLOVE_DECLARE_LOG_CATEGORY(CloveFont)

namespace clove {
    namespace {
        Font::FacePtr makeUniqueFace(FT_Face face) {
            return { face, [](FT_Face face) {
                        FT_Done_Face(face);
                    } };
        }

        Font::FacePtr copyFace(FT_Face face) {
            CLOVE_ASSERT(FT_Reference_Face(face) == FT_Err_Ok);
            return makeUniqueFace(face);
        }
    }

    Font::FTLibWeakPtr Font::ftLib = {};

    Font::Font()
        : graphicsFactory{ Application::get().getGraphicsDevice()->getGraphicsFactory() }
        , face{ nullptr, nullptr } {
        if(ftLib.use_count() == 0) {
            FT_Library library{ nullptr };
            CLOVE_ASSERT(FT_Init_FreeType(&library) == FT_Err_Ok);

            CLOVE_LOG(FreeType, LogLevel::Debug, "Constructed FreeType library");

            auto const libraryDeleter = [](FT_Library lib) {
                CLOVE_LOG(FreeType, LogLevel::Debug, "FreeType library has been deleted");
                FT_Done_FreeType(lib);
            };
            ftLibReference = FTLibSharedPtr{ library, libraryDeleter };
            ftLib          = ftLibReference;
        } else {
            ftLibReference = ftLib.lock();
        }

        face = createFace({ reinterpret_cast<std::byte const *>(roboto_black), roboto_blackLength });
    }

    Font::Font(std::string const &filePath)
        : Font() {
        face = createFace(filePath);
    }

    Font::Font(std::span<std::byte const> bytes)
        : Font() {
        face = createFace(bytes);
    }

    Font::Font(Font const &other)
        : face{ nullptr, nullptr }
        , graphicsFactory{ other.graphicsFactory }
        , ftLibReference{ other.ftLibReference } {
        face = copyFace(other.face.get());
    }

    Font::Font(Font &&other) noexcept = default;

    Font &Font::operator=(Font const &other) {
        graphicsFactory = other.graphicsFactory;
        ftLibReference  = other.ftLibReference;

        face = copyFace(other.face.get());

        return *this;
    }

    Font &Font::operator=(Font &&other) noexcept = default;

    Font::~Font() = default;

    void Font::setFontSize(uint32_t size) {
        FT_Set_Pixel_Sizes(face.get(), 0, static_cast<FT_UInt>(size));
    }

    Font::Glyph Font::getChar(char ch) const {
        int32_t constexpr shift{ 6 };

        FT_Load_Char(face.get(), ch, FT_LOAD_RENDER);

        Glyph glyph{
            .size    = { face->glyph->bitmap.width, face->glyph->bitmap.rows },
            .bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top },
            .advance = { face->glyph->advance.x >> shift, face->glyph->advance.y >> shift },
        };

        unsigned char *faceBuffer{ face->glyph->bitmap.buffer };

        if(faceBuffer != nullptr) {
            size_t constexpr bytesPerTexel{ 1 };//FT uses 1 byte per texel/pixel
            size_t const sizeBytes{ static_cast<size_t>(glyph.size.x * glyph.size.y * bytesPerTexel) };

            GhaImage::Descriptor const glyphImageDescriptor{
                .type        = GhaImage::Type::_2D,
                .usageFlags  = GhaImage::UsageMode::Sampled | GhaImage::UsageMode::TransferDestination,
                .dimensions  = glyph.size,
                .format      = GhaImage::Format::R8_UNORM,//The bitmaps only use the red channel
                .sharingMode = SharingMode::Exclusive,
            };

            glyph.character     = createImageWithData(*graphicsFactory, glyphImageDescriptor, faceBuffer, sizeBytes);
            glyph.characterView = glyph.character->createView(GhaImageView::Descriptor{
                .type       = GhaImageView::Type::_2D,
                .layer      = 0,
                .layerCount = 1,
            });
        }

        return glyph;
    }

    Font::FacePtr Font::createFace(std::string const &filePath) {
        FT_Face face{ nullptr };
        CLOVE_ASSERT(FT_New_Face(ftLibReference.get(), filePath.c_str(), 0, &face) == FT_Err_Ok);

        return makeUniqueFace(face);
    }

    Font::FacePtr Font::createFace(std::span<std::byte const> bytes) {
        FT_Face face{ nullptr };
        auto const *ftBytes{ reinterpret_cast<unsigned char const *>(bytes.data()) };
        CLOVE_ASSERT(FT_New_Memory_Face(ftLibReference.get(), ftBytes, static_cast<FT_Long>(bytes.size_bytes()), 0, &face) == FT_Err_Ok);

        return makeUniqueFace(face);
    }
}