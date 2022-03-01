#include "Membrane/FileSystemHelpers.hpp"

#include "Membrane/MembraneLog.hpp"

#include <Clove/Application.hpp>
#include <Clove/FileSystem/AssetManager.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <filesystem>
#include <msclr/marshal_cppstd.h>
#include <Clove/Application.hpp>
#include <Clove/FileSystem/AssetManager.hpp>

#ifndef GAME_DIR
    #define GAME_DIR
#endif

namespace {
    int32_t convertFileTypeToInt(membrane::FileType type) {
        switch(type) {
            case membrane::FileType::Mesh:
                return 0;
            case membrane::FileType::Texture:
                return 1;
            default:
                return -1;
        }
    }

    membrane::FileType convertIntToFileType(int32_t val){
        switch(val) {
            case 0:
                return membrane::FileType::Mesh;
            case 1:
                return membrane::FileType::Texture;
            default:
                return membrane::FileType::Unknown;
        }
    }

    clove::Guid getAssetGuid(std::filesystem::path const &vfsPath, membrane::FileType type) {
        //Get the Guids through the asset manager to make sure they are valid

        switch(type) {
            case membrane::FileType::Mesh:
                return clove::Application::get().getAssetManager()->getStaticModel(vfsPath).getGuid();
            case membrane::FileType::Texture:
                return clove::Application::get().getAssetManager()->getTexture(vfsPath).getGuid();
            default:
                CLOVE_ASSERT(false);
                return clove::Guid{};
        }
    }
}

namespace membrane {
    System::String ^ FileSystemHelpers::getContentPath() {
        return gcnew System::String{ GAME_DIR "/content" };
    }

    System::String ^ FileSystemHelpers::getAssetExtension() {
        return gcnew System::String{ ".clvasset" };
    }

    bool FileSystemHelpers::isFileSupported(System::String ^ file) {
        System::String ^ extension { System::IO::Path::GetExtension(file) };

        return extension == ".clvasset" ||
            extension == ".obj" ||
            extension == ".fbx" ||
            extension == ".png" ||
            extension == ".jpg";
    }

    bool FileSystemHelpers::isAssetFile(System::String ^ file) {
        return System::IO::Path::GetExtension(file) == ".clvasset";
    }

    FileType FileSystemHelpers::getFileType(System::String ^ fullFilePath) {
        if(!isFileSupported(fullFilePath)) {
            throw gcnew System::ArgumentException("File is not a type that is supported by Clove", fullFilePath);
        }

        System::String ^ extension { System::IO::Path::GetExtension(fullFilePath) };

        if(extension == ".clvasset") {
            return getAssetFileType(fullFilePath);
        } else {
            if(extension == ".obj" ||
               extension == ".fbx") {
                return FileType::Mesh;
            } else if(extension == ".png" ||
                      extension == ".jpg") {
                return FileType::Texture;
            }
        }

        CLOVE_ASSERT_MSG(false, "{0} is unable to determine file type", CLOVE_FUNCTION_NAME_PRETTY);
        return FileType::Mesh;
    }

    void FileSystemHelpers::createAssetFile(System::String ^ location, System::String ^ fullPath, System::String ^ relPath, System::String ^ vfsPath) {
        std::filesystem::path const saveLocation{ msclr::interop::marshal_as<std::string>(location) };
        std::filesystem::path const relativePath{ msclr::interop::marshal_as<std::string>(relPath) };
        std::filesystem::path const nativeVfsPath{ msclr::interop::marshal_as<std::string>(vfsPath) };

        FileType const type{ getFileType(fullPath) };
        std::ofstream fileStream{ saveLocation, std::ios::out | std::ios::trunc };

        {
            clove::serialiser::Node rootNode{};
            rootNode["asset"]["file_version"] = 1;
            rootNode["asset"]["type"]         = convertFileTypeToInt(type);
            rootNode["asset"]["path"]         = relativePath.string();

            fileStream << clove::emittYaml(rootNode);
            fileStream.flush();

            rootNode["asset"]["guid"] = static_cast<clove::Guid::Type>(getAssetGuid(nativeVfsPath, type));

            fileStream.seekp(0);
            fileStream << clove::emittYaml(rootNode);
        }
    }

    void FileSystemHelpers::removeAssetFile(System::UInt64 assetGuid, FileType assetFileType) {
        clove::Guid const guid{ assetGuid };

        switch(assetFileType) {
            case FileType::Mesh:
                clove::Application::get().getAssetManager()->removeStaticModel(guid);
                break;
            case FileType::Texture:
                clove::Application::get().getAssetManager()->removeTexture(guid);
                break;
            default:
                CLOVE_ASSERT(false);
            case FileType::Unknown:
                break;
        }
    }

    FileType FileSystemHelpers::getAssetFileType(System::String ^ fullFilePath) {
        if(!isAssetFile(fullFilePath)) {
            return FileType::Unknown;
        }

        std::filesystem::path const path{ msclr::interop::marshal_as<std::string>(fullFilePath) };
        clove::serialiser::Node fileNode{ clove::loadYaml(path).getValue() };

        return convertIntToFileType(fileNode["asset"]["type"].as<int32_t>());
    }

    System::UInt64 FileSystemHelpers::getAssetFileGuid(System::String ^ fullFilePath) {
        if(!isAssetFile(fullFilePath)) {
            return 0;
        }

        std::filesystem::path const path{ msclr::interop::marshal_as<std::string>(fullFilePath) };
        clove::serialiser::Node fileNode{ clove::loadYaml(path).getValue() };

        return fileNode["asset"]["guid"].as<uint64_t>();
    }
}