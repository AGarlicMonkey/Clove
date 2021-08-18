#include "Clove/Rendering/Renderables/StaticModel.hpp"

#include "Clove/Rendering/Renderables/Mesh.hpp"
#include "Clove/Rendering/Techniques/ForwardLightingTechnique.hpp"

namespace clove {
    StaticModel::StaticModel(std::vector<std::shared_ptr<Mesh>> meshes)
        : StaticModel{ std::move(meshes), { createForwardLightingTechnique() } } {
    }

    StaticModel::StaticModel(std::vector<std::shared_ptr<Mesh>> meshes, std::vector<Technique> renderingTechniques)
        : meshes{ std::move(meshes) }
        , techniques{ std::move(renderingTechniques) } {
    }

    StaticModel::StaticModel(StaticModel const &other)
        : techniques{ other.techniques } {
        meshes.clear();
        for(auto const &mesh : other.meshes) {
            meshes.emplace_back(std::make_shared<Mesh>(*mesh));
        }
    }

    StaticModel::StaticModel(StaticModel &&other) noexcept = default;

    StaticModel &StaticModel::operator=(StaticModel const &other) {
        techniques = other.techniques;
        meshes.clear();
        for(auto const &mesh : other.meshes) {
            meshes.emplace_back(std::make_shared<Mesh>(*mesh));
        }

        return *this;
    }

    StaticModel &StaticModel::operator=(StaticModel &&other) noexcept = default;

    StaticModel::~StaticModel() = default;
}