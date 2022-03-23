#include "Membrane/FileSystemHelpers.hpp"

#include <Clove/Application.hpp>
#include <Clove/FileSystem/AssetManager.hpp>
#include <Clove/FileSystem/VirtualFileSystem.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <fstream>

#ifndef GAME_DIR
    #define GAME_DIR
#endif

using namespace clove;

namespace {
    //Required to mirror convertFileTypeToInt in FileSystemHelpers.cs
    constexpr int32_t FILETYPE_UNKNOWN{ -1 };
    constexpr int32_t FILETYPE_MESH{ 0 };
    constexpr int32_t FILETYPE_TEXTURE{ 1 };

    Guid getAssetGuid(std::filesystem::path const &vfsPath, int32_t type) {
        //Get the Guids through the asset manager to make sure they are valid

        switch(type) {
            case FILETYPE_MESH:
                return Application::get().getAssetManager()->getStaticModel(vfsPath).getGuid();
            case FILETYPE_TEXTURE:
                return Application::get().getAssetManager()->getTexture(vfsPath).getGuid();
            default:
                CLOVE_ASSERT(false);
                return Guid{};
        }
    }
}

BSTR getContentPath() {
    return SysAllocString(GAME_DIR L"/content");
}

bool isFileSupported(wchar_t const *file) {
    std::string const extension{ std::filesystem::path{ file }.extension().string() };

    return extension == ".clvasset" ||
        extension == ".obj" ||
        extension == ".fbx" ||
        extension == ".png" ||
        extension == ".jpg";
}

bool isAssetFile(wchar_t const *file) {
    return std::filesystem::path{ file }.extension().string() == ".clvasset";
}

int32_t getFileType(wchar_t const *file) {
    if(!isFileSupported(file)) {
        throw std::runtime_error{ "File is not a type that is supported by Clove" };
    }

    std::filesystem::path const filePath{ file };
    std::string const extension{ filePath.extension().string() };

    if(extension == ".clvasset") {
        return getAssetFileType(file);
    } else {
        if(extension == ".obj" ||
           extension == ".fbx") {
            return FILETYPE_MESH;
        } else if(extension == ".png" ||
                  extension == ".jpg") {
            return FILETYPE_TEXTURE;
        }
    }

    CLOVE_ASSERT_MSG(false, "Unable to determine file type of {0}", filePath.string());
    return FILETYPE_UNKNOWN;
}

void createAssetFile(wchar_t const *assetLocation, wchar_t const *fileToCreateFrom, wchar_t const *relPathOfCreateFrom, wchar_t const *assetVfsPath) {
    std::filesystem::path const saveLocation{ assetLocation };
    std::filesystem::path const relativePath{ fileToCreateFrom };
    std::filesystem::path const nativeVfsPath{ relPathOfCreateFrom };

    int32_t const type{ getFileType(fileToCreateFrom) };
    std::ofstream fileStream{ saveLocation, std::ios::out | std::ios::trunc };

    {
        serialiser::Node rootNode{};
        rootNode["asset"]["file_version"] = 1;
        rootNode["asset"]["type"]         = type;
        rootNode["asset"]["path"]         = relativePath.string();

        fileStream << emittYaml(rootNode);
        fileStream.flush();

        rootNode["asset"]["guid"] = static_cast<Guid::Type>(getAssetGuid(nativeVfsPath, type));

        fileStream.seekp(0);
        fileStream << emittYaml(rootNode);
    }
}

void moveAssetFile(wchar_t const *sourceFileName, wchar_t const *destFileName) {
    std::filesystem::path const source{ sourceFileName };
    std::filesystem::path const dest{ destFileName };

    serialiser::Node assetNode{ loadYaml(dest).getValue() };
    std::filesystem::path const assetPath{ (source.parent_path() / assetNode["asset"]["path"].as<std::wstring>()).lexically_normal() };

    //Make sure to update the new relative path
    assetNode["asset"]["path"] = std::filesystem::relative(assetPath, dest.parent_path()).string();
    {
        CLOVE_ASSERT(std::filesystem::exists(dest));
        std::ofstream fileStream{ dest, std::ios::out | std::ios::trunc };
        fileStream << emittYaml(assetNode);
    }

    //Convert these into the proper vfs paths (folder/model.obj etc.) as the editor will always deal with absolute paths.
    VirtualFileSystem::Path const sourceVfs{ std::filesystem::relative(source, GAME_DIR "/content").replace_extension(assetPath.extension()) };
    VirtualFileSystem::Path const destVfs{ std::filesystem::relative(dest, GAME_DIR "/content").replace_extension(assetPath.extension()) };

    //Notify the asset manager of the move
    switch(getFileType(destFileName)) {
        case FILETYPE_MESH:
            Application::get().getAssetManager()->moveStaticModel(sourceVfs, destVfs);
            break;
        case FILETYPE_TEXTURE:
            Application::get().getAssetManager()->moveTexture(sourceVfs, destVfs);
            break;
        default:
            CLOVE_ASSERT(false);
        case FILETYPE_UNKNOWN:
            break;
    }
}

void removeAssetFile(uint64_t assetGuid, int32_t assetFileType) {
    Guid const guid{ assetGuid };

    switch(assetFileType) {
        case FILETYPE_MESH:
            Application::get().getAssetManager()->removeStaticModel(guid);
            break;
        case FILETYPE_TEXTURE:
            Application::get().getAssetManager()->removeTexture(guid);
            break;
        default:
            CLOVE_ASSERT(false);
        case FILETYPE_UNKNOWN:
            break;
    }
}

int32_t getAssetFileType(wchar_t const *fullFilePath) {
    if(!isAssetFile(fullFilePath)) {
        return FILETYPE_UNKNOWN;
    }

    std::filesystem::path const path{ fullFilePath };
    serialiser::Node fileNode{ loadYaml(path).getValue() };

    return fileNode["asset"]["type"].as<int32_t>();
}

Guid::Type getAssetFileGuid(wchar_t const *fullFilePath) {
    if(!isAssetFile(fullFilePath)) {
        return 0;
    }

    std::filesystem::path const path{ fullFilePath };
    serialiser::Node fileNode{ loadYaml(path).getValue() };

    return fileNode["asset"]["guid"].as<Guid::Type>();
}