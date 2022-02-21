#include "Membrane/EditorVFS.hpp"

#include <Clove/Log/Log.hpp>
#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>

CLOVE_DECLARE_LOG_CATEGORY(MembraneEditorVFS)

using namespace clove;

namespace membrane {
    std::filesystem::path EditorVFS::resolve(Path const &path) {
        std::filesystem::path assetPath{ contentDirectory / path };
        std::filesystem::path systemPath{};

        //TEMP: Special case for scene files
        //TODO: Treat scenes as asset files as well? Use ["asset"]["type"] to differentiate.
        if(assetPath.extension() == ".clvscene") {
            return assetPath.lexically_normal();
        }

        assetPath.replace_extension(".clvasset");

        if(std::filesystem::exists(assetPath)) {
            serialiser::Node assetNode{ loadYaml(assetPath).getValue() };

            std::filesystem::path const relativePath{ assetNode["asset"]["path"].as<std::string>() };
            systemPath = contentDirectory / relativePath;

            CLOVE_LOG(MembraneEditorVFS, LogLevel::Trace, "Resolved {0} into {1}", path.string(), systemPath.lexically_normal().string());
        } else {
            CLOVE_LOG(MembraneEditorVFS, LogLevel::Error, "Could not find asset file {0}", assetPath.string());
            throw std::runtime_error("Could not load asset file. See log");
        }

        return systemPath.lexically_normal();
    }
}