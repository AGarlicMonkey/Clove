namespace clove {
    void Image::setPosition(vec2f position) {
        this->position = std::move(position);
    }

    void Image::setRotation(float rotation) {
        this->rotation = rotation;
    }

    void Image::setSize(vec2f size) {
        this->size = std::move(size);
    }

    vec2f const &Image::getPosition() const {
        return position;
    }

    float Image::getRotation() const {
        return rotation;
    }

    vec2f const &Image::getSize() const {
        return size;
    }
}