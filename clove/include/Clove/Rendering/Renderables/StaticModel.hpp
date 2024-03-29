#pragma once

#include <filesystem>
#include <memory>
#include <vector>

namespace clove {
    class Mesh;
}

namespace clove {
    /**
     * @brief Represents a collection of meshes.
     */
    class StaticModel {
        //VARIABLES
    private:
        std::vector<std::shared_ptr<Mesh>> meshes;

        //FUNCTIONS
    public:
        StaticModel() = delete;
        StaticModel(std::vector<std::shared_ptr<Mesh>> meshes);

        StaticModel(StaticModel const &other);
        StaticModel(StaticModel &&other) noexcept;

        StaticModel &operator=(StaticModel const &other);
        StaticModel &operator=(StaticModel &&other) noexcept;

        ~StaticModel();

        inline std::vector<std::shared_ptr<Mesh>> const &getMeshes() const;

        inline std::shared_ptr<Mesh> &operator[](size_t index);
    };
}

#include "StaticModel.inl"