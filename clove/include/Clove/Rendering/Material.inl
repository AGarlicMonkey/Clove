namespace clove {
    void Material::setDiffuseTexture(AssetPtr<Texture> texture) {
        diffuseTexture = std::move(texture);
    }

    void Material::setSpecularTexture(AssetPtr<Texture> texture) {
        specularTexture = std::move(texture);
    }

    void Material::setColour(vec4f colour) {
        this->colour = std::move(colour);
    }

    vec4f const &Material::getColour() const {
        return colour;
    }

    void Material::setShininess(float shininess) {
        this->shininess = shininess;
    }

    float Material::getShininess() const {
        return shininess;
    }
}