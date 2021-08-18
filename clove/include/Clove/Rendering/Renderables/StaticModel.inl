namespace clove {
    std::vector<std::shared_ptr<Mesh>> const &StaticModel::getMeshes() const {
        return meshes;
    }

    std::vector<Technique> const &StaticModel::getTechniques() const {
        return techniques;
    }

    void StaticModel::addTechnique(Technique technique) {
        techniques.emplace_back(std::move(technique));
    }

    void StaticModel::removeTechnique(Technique const &technique) {
        techniques.erase(std::remove(techniques.begin(), techniques.end(), technique));
    }

    std::shared_ptr<Mesh> &StaticModel::operator[](size_t index) {
        return meshes[index];
    }
}