#pragma once

#include "Clove/Rendering/RenderingConstants.hpp"

#include <Clove/Maths/Matrix.hpp>
#include <Clove/DeltaTime.hpp>
#include <array>

namespace garlic::clove {
    class AnimationClip;
    class AnimationPose;
}

namespace garlic::clove {
    /**
     * @brief Drives the animation for an AnimatedModel.
     * @details Takes in an AnimationClip and uses that to calculate an AnimatedModel's skinning matrix palette for a given frame.
     */
    class Animator {
        //VARIABLES
    private:
        float currentTime{ 0.0f };
        AnimationClip *currentClip{ nullptr };

        //FUNCTIONS
    public:
        Animator();

        Animator(Animator const &other);
        Animator(Animator &&other) noexcept;

        Animator &operator=(Animator const &other);
        Animator &operator=(Animator &&other) noexcept;

        ~Animator();

        /**
         * @brief Updates the Animator, generating a matrix palette.
         * @param deltaTime Time since last frame.
         * @returns The matrix palette for a given frame.
         */
        std::array<mat4f, MAX_JOINTS> update(DeltaTime const deltaTime);

        /**
         * @brief Sets the current clip the Animator will use
         * @param clip Pointer to the new clip
         */
        void setCurrentClip(AnimationClip *clip);

    private:
        std::pair<AnimationPose const &, AnimationPose const &> getPrevNextPose(float animationTime);
    };
}