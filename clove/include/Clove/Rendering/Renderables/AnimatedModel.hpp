#pragma once

#include "Clove/Rendering/AnimationTypes.hpp"
#include "Clove/Rendering/Animator.hpp"
#include "Clove/Rendering/Renderables/StaticModel.hpp"

#include <Clove/Maths/Vector.hpp>
#include <Clove/DeltaTime.hpp>
#include <vector>
#include <memory>

namespace clove {
    /**
     * @brief Represents a collection of meshes that can be animated.
     */
    class AnimatedModel : public StaticModel {
        //VARIABLES
    private:
        Animator animator;

        std::unique_ptr<Skeleton> skeleton;
        std::vector<AnimationClip> animClips;

        //FUNCTIONS
    public:
        AnimatedModel() = delete;
        AnimatedModel(std::vector<std::shared_ptr<Mesh>> meshes, std::unique_ptr<Skeleton> skeleton, std::vector<AnimationClip> animClips);

        AnimatedModel(AnimatedModel const &other);
        AnimatedModel(AnimatedModel &&other) noexcept;

        AnimatedModel &operator=(AnimatedModel const &other);
        AnimatedModel &operator=(AnimatedModel &&other) noexcept;

        ~AnimatedModel();

        /**
         * @brief Updates the internal animator.
         * @param deltaTime The time since the last frame
         * @returns The matrix palette for the skeleton for a given frame
         */
        inline std::array<mat4f, MAX_JOINTS> update(DeltaTime const deltaTime);
    };
}

#include "AnimatedModel.inl"