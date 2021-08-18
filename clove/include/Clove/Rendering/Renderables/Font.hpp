#pragma once

#include <Clove/Maths/Vector.hpp>
#include <memory>
#include <span>
#include <string>

//Forward dec (see freetype.h)
typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;

namespace clove {
    class GhaFactory;
    class GhaImage;
    class GhaImageView;
}

namespace clove {
    class Font {
        //TYPES
    public:
        struct Glyph {
            vec2ui size{};
            vec2i bearing{};
            vec2i advance{};
            std::shared_ptr<GhaImage> character;
            std::shared_ptr<GhaImageView> characterView;
        };

        using FTLibWeakPtr   = std::weak_ptr<std::remove_pointer_t<FT_Library>>;
        using FTLibSharedPtr = std::shared_ptr<std::remove_pointer_t<FT_Library>>;

        using FacePtr = std::unique_ptr<std::remove_pointer_t<FT_Face>, void (*)(FT_Face)>;

        //VARIABLES
    private:
        std::shared_ptr<GhaFactory> graphicsFactory;

        static FTLibWeakPtr ftLib;
        FTLibSharedPtr ftLibReference;

        FacePtr face;

        //FUNCTIONS
    public:
        Font();
        Font(std::string const &filePath);
        Font(std::span<std::byte const> bytes);

        Font(Font const &other);
        Font(Font &&other) noexcept;

        Font &operator=(Font const &other);
        Font &operator=(Font &&other) noexcept;

        ~Font();

        void setFontSize(uint32_t size);
        Glyph getChar(char ch) const;

    private:
        FacePtr createFace(std::string const &filePath);
        FacePtr createFace(std::span<std::byte const> bytes);
    };
}