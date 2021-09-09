#pragma once

#include "Clove/Rendering/AnimationTypes.hpp"
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

        inline std::vector<AnimationClip> const &getAnimationClips() const;
    };
}

#include "AnimatedModel.inl"