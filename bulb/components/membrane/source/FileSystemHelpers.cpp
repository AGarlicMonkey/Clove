#include "Membrane/FileSystemHelpers.hpp"

#include "Membrane/MembraneLog.hpp"

#include <Clove/Log/Log.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <filesystem>
#include <msclr/marshal_cppstd.h>
#include <Clove/Application.hpp>

#ifndef GAME_DIR
#define GAME_DIR
#endif

namespace {
    int32_t convertFileTypeToInt(membrane::FileType type) {
        switch (type) {
            case membrane::FileType::Mesh:
                return 0;
            case membrane::FileType::Texture:
                return 1;
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

    bool FileSystemHelpers::isFileSupported(System::String ^file) {
        System::String ^extension{ System::IO::Path::GetExtension(file) };

        return  extension == ".clvasset" ||
                extension == ".obj" ||
                extension == ".fbx" ||
                extension == ".png" ||
                extension == ".jpg";
    }

    bool FileSystemHelpers::isAssetFile(System::String ^ file) {
        return System::IO::Path::GetExtension(file) == ".clvasset";
    }

    FileType FileSystemHelpers::getFileType(System::String ^file) {
        if(!isFileSupported(file)) {
            throw gcnew System::ArgumentException("File is not a type that is supported by Clove", file);
        }

        System::String ^extension { System::IO::Path::GetExtension(file) };

        if(extension == ".clvasset") {
            return getAssetFileType(file);
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

    FileType FileSystemHelpers::getAssetFileType(System::String ^file) {
        throw gcnew System::NotImplementedException(); //TODO
    }

    void FileSystemHelpers::createAssetFile(System::String ^ location, System::String ^ relPath, System::String ^ vfsPath) {
        std::filesystem::path const saveLocation{ msclr::interop::marshal_as<std::string>(location) };

        std::filesystem::path const relativePath{ msclr::interop::marshal_as<std::string>(relPath) };
        std::filesystem::path const loadPath{ msclr::interop::marshal_as<std::string>(vfsPath) };

        clove::serialiser::Node rootNode{};
        rootNode["asset"]["file_version"] = 1;
        rootNode["asset"]["type"]         = convertFileTypeToInt(getFileType(vfsPath));
        rootNode["asset"]["path"]         = relativePath.string();

        std::ofstream fileStream{ saveLocation, std::ios::out | std::ios::trunc };
        fileStream << clove::emittYaml(rootNode);
    }
}