namespace clove {
    void Text::setPosition(vec2f position) {
        this->position = std::move(position);
    }

    void Text::setRotation(float rotation) {
        this->rotation = rotation;
    }

    void Text::setScale(vec2f scale) {
        textScale = std::move(scale);
    }

    vec2f const &Text::getPosition() const {
        return position;
    }

    float Text::getRotation() const {
        return rotation;
    }

    vec2f const &Text::getScale() const {
        return textScale;
    }

    void Text::setText(std::string text) {
        this->text    = std::move(text);
        isBufferDirty = true;
    }

    void Text::setFontSize(uint32_t const size) {
        font.setFontSize(size);
        isBufferDirty = true;
    }

    std::size_t Text::getTextLength() const {
        return text.length();
    }
}