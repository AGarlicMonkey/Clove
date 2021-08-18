#pragma once

#include "Clove/Rendering/ShaderBufferTypes.hpp"

#include <Clove/Maths/Matrix.hpp>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace clove {
    class GhaDescriptorSet;
    class GhaGraphicsCommandBuffer;
    class GhaComputeCommandBuffer;
    class Mesh;
}

namespace clove {
    /**
     * @brief A GeometryPass is a single graphics operation that is performed on a set of meshes.
     */
    class GeometryPass {
        //TYPES
    public:
        using Id = size_t;

        /**
         * @brief Data a GeometryPass will need for a given frame.
         */
        struct FrameData {
            std::vector<std::shared_ptr<GhaDescriptorSet>> meshDescriptorSets{}; /**< Descriptor set for each mesh submitted for the frame. */
            std::vector<std::shared_ptr<GhaDescriptorSet>> skinningMeshSets{};

            std::shared_ptr<GhaDescriptorSet> viewDescriptorSet{ nullptr };     /**< Descriptor set for view specific data. */
            std::shared_ptr<GhaDescriptorSet> lightingDescriptorSet{ nullptr }; /**< Descriptor set for lighting specific data. */

            //TODO: This is specific to the light passes. Should it be in here?
            mat4f *currentDirLightTransform{ nullptr };

            mat4f *currentPointLightTransform{ nullptr };
            vec3f currentPointLightPosition{};
            float currentPointLightFarPlane{};
        };

        /**
         * @brief A single unit of work for a GeometryPass.
         */
        struct Job {
            size_t meshDescriptorIndex{ 0 };      /**< Index into FrameData::meshDescriptorSets */
            std::shared_ptr<Mesh> mesh{ nullptr };//TODO: Move into frame data
        };

        //VARIABLES
    private:
        std::vector<Job> jobs{};

        //FUNCTIONS
    public:
        GeometryPass();

        GeometryPass(GeometryPass const &other);
        GeometryPass(GeometryPass &&other) noexcept;

        GeometryPass &operator=(GeometryPass const &other);
        GeometryPass &operator=(GeometryPass &&other) noexcept;

        virtual ~GeometryPass();

        template<typename GeometryPassType>
        static Id getId();

        /**
         * @brief Adds a job to this pass' queue.
         */
        void addJob(Job job);

        /**,
         * @brief Clears the job queue
         */
        void flushJobs();

        /**
         * @brief Submits all jobs into the commandBuffer.
         * @param commandBuffer GhaGraphicsCommandBuffer to record commands into.
         * @param frameData Data that describes the current frame.
         */
        virtual void execute(GhaGraphicsCommandBuffer &commandBuffer, FrameData const &frameData) {}/* = 0; */

        //TEMP: GeometryPass should really allocate it's own command buffer
        virtual void execute(GhaComputeCommandBuffer &commandBuffer, FrameData const &frameData) {}/* = 0; */

    protected:
        inline std::vector<Job> const &getJobs() const;
    };
}

#include "GeometryPass.inl"