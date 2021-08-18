namespace clove {
    std::shared_ptr<GhaBuffer> const &Mesh::getVertexBuffer() const {
        return vertexBuffer;
    }

    std::shared_ptr<GhaBuffer> const &Mesh::getCombinedBuffer() const {
        return combinedBuffer;
    }

    size_t Mesh::getVertexOffset() const {
        return vertexOffset;
    }

    size_t Mesh::getVertexBufferSize() const {
        return vertexBufferSize;
    }

    size_t Mesh::getVertexCount() const {
        return vertices.size();
    }

    size_t Mesh::getIndexOffset() const {
        return indexOffset;
    }

    size_t Mesh::getIndexCount() const {
        return indices.size();
    }
}