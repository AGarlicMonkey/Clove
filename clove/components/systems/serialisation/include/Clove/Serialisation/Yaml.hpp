#pragma once

#include "Clove/Serialisation/Node.hpp"

#include <Clove/Expected.hpp>
#include <string>
#include <filesystem>

namespace clove{
    enum class LoadError {
        BadFile,
        WrongType,
        WrongVersion,
    };

    /**
     * @brief Emitts a node graph as a yaml file.
     */
    std::string emittYaml(serialiser::Node const &node);

    /**
     * @brief Loads a yaml file into a node graph.
     */
    Expected<serialiser::Node, LoadError> loadYaml(std::filesystem::path const &filePath);
}