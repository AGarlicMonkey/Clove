namespace garlic::clove {
    bool RgBuffer::isExternalBuffer() const {
        return isExternalBuffer;
    }

    size_t RgBuffer::getBufferOffset() const {
        return offset;
    }

    size_t RgBuffer::getBufferSize() const {
        return size;
    }
}