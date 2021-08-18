#pragma once

#include "Clove/Rendering/Vertex.hpp"

namespace clove {
    class GhaBuffer;
    class VertexLayout;
    class VertexBufferData;
    class CommandBuffer;
    class GhaFactory;
}

namespace clove {
    class Mesh {
        //VARIABLES
    private:
        /**
         * @brief Buffer containing the bind pose vertices of this mesh.
         */
        std::shared_ptr<GhaBuffer> vertexBuffer;
        /**
         * @brief Buffer container both the indices and skinned vertices of this mesh.
         */
        std::shared_ptr<GhaBuffer> combinedBuffer;

        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;

        size_t vertexOffset{ 0 };
        size_t vertexBufferSize{ 0 };
        size_t indexOffset{ 0 };

        //FUNCTIONS
    public:
        Mesh() = delete;
        Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices);

        Mesh(Mesh const &other);
        Mesh(Mesh &&other) noexcept;

        Mesh &operator=(Mesh const &other);
        Mesh &operator=(Mesh &&other) noexcept;

        ~Mesh();

        /**
         * @brief Returns the GhaBuffer containing just the vertices.
         * @details This buffer contains the bind pose vertices for the
         * mesh. Before any skinning has taken place.
         */
        inline std::shared_ptr<GhaBuffer> const &getVertexBuffer() const;

        /**
         * @brief Returns a GhaBuffer containing both vertices and indices.
         * @details The vertices in this buffer are pre-skinned and are safe
         * to use straight away for animated meshes.
         */
        inline std::shared_ptr<GhaBuffer> const &getCombinedBuffer() const;

        /**
         * @brief Returns the offset into the combinedBuffer for the vertices.
         */
        inline size_t getVertexOffset() const;
        inline size_t getVertexBufferSize() const;
        inline size_t getVertexCount() const;

        /**
         * @brief Returns the offset into the combinedBuffer for the indices.
         */
        inline size_t getIndexOffset() const;
        inline size_t getIndexCount() const;
    };
}

#include "Mesh.inl"
