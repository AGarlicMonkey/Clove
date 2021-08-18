#pragma once

#include <Clove/Maths/Matrix.hpp>
#include <Clove/Maths/Quaternion.hpp>
#include <Clove/Maths/Vector.hpp>
#include <cinttypes>
#include <optional>
#include <string>
#include <vector>

namespace clove {
    using JointIndexType = uint8_t;

    struct Joint {
        std::string name;
        mat4f inverseBindPose{ 1.0f }; /**< Bm->j */
        std::optional<JointIndexType> parentIndex{};
    };

    struct Skeleton {
        std::vector<Joint> joints{};
    };

    /**
     * @brief The position that the joint will be in when this pose is applied to it (in joint space relative to parent)
     */
    struct JointPose {
        quatf rotation{ vec3f{ 0.0f } };
        vec3f position{ 0.0f };
        vec3f scale{ 1.0f };
    };

    /**
     * @brief The pose for an entire skeleton, a keyframe in the animation
     */
    struct AnimationPose {
        float timeStamp{ 0.0f };
        std::vector<JointPose> poses{}; /**< one for each joint */
    };

    /**
     * @brief An entire animation (like walk or run)
     */
    struct AnimationClip {
        Skeleton *skeleton{ nullptr };
        float duration{ 0.0f };
        std::vector<AnimationPose> poses{}; /**< all the poses for this clip */
    };
}