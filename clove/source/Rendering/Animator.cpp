#include "Clove/Rendering/Animator.hpp"

#include "Clove/Rendering/AnimationTypes.hpp"

#include <Clove/Definitions.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Maths/MathsHelpers.hpp>
#include <Clove/Timer.hpp>
#include <cmath>

namespace clove {
    namespace {
        std::vector<JointPose> lerpJointPoses(AnimationPose const &posesA, AnimationPose const &posesB, float const time) {
            size_t const jointCount{ posesA.poses.size() };

            std::vector<JointPose> lerpedPoses;
            lerpedPoses.resize(jointCount);

            for(size_t i = 0; i < jointCount; ++i) {
                auto const pos{ lerp(posesA.poses[i].position, posesB.poses[i].position, time) };
                auto const rot{ slerp(posesA.poses[i].rotation, posesB.poses[i].rotation, time) };
                auto const scale{ lerp(posesA.poses[i].scale, posesB.poses[i].scale, time) };

                lerpedPoses[i] = { rot, pos, scale };
            }

            return lerpedPoses;
        }

        mat4f getJointToModelMatrix(JointPose const &pose, std::optional<JointIndexType> const &parentIndex, std::vector<JointPose> const &poses, Skeleton const *skeleton) {
            //TODO: A lot of recalculations here, perhaps we can make it so we don't revisit joints once they are done

            mat4f const translationMatrix{ translate(mat4f(1.0f), pose.position) };
            mat4f const rotationMatrix{ quaternionToMatrix4(pose.rotation) };
            mat4f const scaleMatrix{ scale(mat4f(1.0f), pose.scale) };

            mat4f const poseTransform{ translationMatrix * rotationMatrix * scaleMatrix };

            if(parentIndex.has_value()) {
                JointIndexType const index{ parentIndex.value() };
                mat4f const parentPoseTransform{ getJointToModelMatrix(poses[index], skeleton->joints[index].parentIndex, poses, skeleton) };

                return parentPoseTransform * poseTransform;
            } else {
                return poseTransform;
            }
        }

        std::vector<mat4f> calculateCurrentJointToModelMatrices(std::vector<JointPose> const &poses, Skeleton const *skeleton) {
            size_t const size{ poses.size() };
            std::vector<mat4f> jointToModelMatrices(size);
            for(size_t i = 0; i < size; ++i) {
                jointToModelMatrices[i] = getJointToModelMatrix(poses[i], skeleton->joints[i].parentIndex, poses, skeleton);
            }
            return jointToModelMatrices;
        }
    }

    Animator::Animator() = default;

    Animator::Animator(Animator const &other) = default;

    Animator::Animator(Animator &&other) noexcept = default;

    Animator &Animator::operator=(Animator const &other) = default;

    Animator &Animator::operator=(Animator &&other) noexcept = default;

    std::array<mat4f, MAX_JOINTS> Animator::update(DeltaTime const &deltaTime) {
        CLOVE_PROFILE_FUNCTION();

        std::array<mat4f, MAX_JOINTS> skinningMatrix{};
        skinningMatrix.fill(mat4f{ 1.0f });

        if(currentClip == nullptr) {
            return skinningMatrix;
        }

        //TODO: single play (current will loop)
        currentTime = std::fmod(currentTime + deltaTime, currentClip->duration);

        //Get the poses either side of the animation time
        auto const &[prevPose, nextPose] = getPrevNextPose(currentTime);

        //Lerp between them to create the target pose
        float const timeBetweenPoses{ nextPose.timeStamp - prevPose.timeStamp };
        float const timeFromPrevPose{ currentTime - prevPose.timeStamp };
        float const normTime{ timeFromPrevPose / timeBetweenPoses };

        auto const currentAnimPose{ lerpJointPoses(prevPose, nextPose, normTime) };

        //Get the current joint to model matrix of the target pose (Cj->m)
        auto const currentJointToModel{ calculateCurrentJointToModelMatrices(currentAnimPose, currentClip->skeleton) };

        //Calculate skinning matrix K = Cj->m * Bm->j (right to left)
        for(size_t i = 0; i < currentJointToModel.size(); ++i) {
            skinningMatrix[i] = currentJointToModel[i] * currentClip->skeleton->joints[i].inverseBindPose;
        }

        return skinningMatrix;
    }

    void Animator::setCurrentClip(AnimationClip *clip) {
        currentClip = clip;
    }

    std::pair<AnimationPose const &, AnimationPose const &> Animator::getPrevNextPose(float animationTime) {
        size_t prevIndex{ 0 };
        size_t nextIndex{ 0 };

        for(size_t i = 0; i < currentClip->poses.size(); ++i) {
            nextIndex = i;
            if(currentClip->poses[i].timeStamp > animationTime) {
                break;
            }
            prevIndex = i;
        }

        return { currentClip->poses[prevIndex], currentClip->poses[nextIndex] };
    }
}