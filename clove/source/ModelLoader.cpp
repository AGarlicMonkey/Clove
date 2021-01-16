#include "Clove/ModelLoader.hpp"

#include "Clove/Rendering/Renderables/Mesh.hpp"

#include <Clove/Expected.hpp>
#include <Clove/Log/Log.hpp>
#include <Clove/Maths/Maths.hpp>
#include <Clove/Maths/Matrix.hpp>
#include <Clove/Maths/Quaternion.hpp>
#include <Clove/Maths/Vector.hpp>
#include <Clove/Platform/Window.hpp>
#include <Clove/Timer.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <map>
#include <set>

namespace garlic::clove::ModelLoader {
    namespace {
        mat4f convertToGarlicMatrix(aiMatrix4x4 const &aiMat) {
            mat4f garlicMat;

            garlicMat[0][0] = aiMat.a1;
            garlicMat[0][1] = aiMat.b1;
            garlicMat[0][2] = aiMat.c1;
            garlicMat[0][3] = aiMat.d1;

            garlicMat[1][0] = aiMat.a2;
            garlicMat[1][1] = aiMat.b2;
            garlicMat[1][2] = aiMat.c2;
            garlicMat[1][3] = aiMat.d2;

            garlicMat[2][0] = aiMat.a3;
            garlicMat[2][1] = aiMat.b3;
            garlicMat[2][2] = aiMat.c3;
            garlicMat[2][3] = aiMat.d3;

            garlicMat[3][0] = aiMat.a4;
            garlicMat[3][1] = aiMat.b4;
            garlicMat[3][2] = aiMat.c4;
            garlicMat[3][3] = aiMat.d4;

            return garlicMat;
        }

        vec3f convertToGarlicVec(aiVector3D const &aiVec) {
            return { aiVec.x, aiVec.y, aiVec.z };
        }

        quatf convertToGarlicQuat(aiQuaternion const &aiQuat) {
            return { aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z };
        }

        void buildNodeNameMap(std::unordered_map<std::string, aiNode *> &map, aiNode *rootNode) {
            map[rootNode->mName.C_Str()] = rootNode;
            for(size_t i = 0; i < rootNode->mNumChildren; ++i) {
                buildNodeNameMap(map, rootNode->mChildren[i]);
            }
        }

        std::optional<size_t> getPreviousIndex(AnimationClip const &clip, float frame, float framesPerSecond, JointIndexType jointIndex, std::map<float, size_t> &framePoseIndexMap, std::map<float, std::vector<JointIndexType>> &missingPoseMap) {
            size_t const currIndex{ framePoseIndexMap[frame] };

            for(int i = currIndex - 1; i >= 0; --i) {
                float const poseFrame{ clip.poses[i].timeStamp * framesPerSecond };//Get the time of the pose before this one to check if it's not a missing pose
                auto const &missingPoses{ missingPoseMap[poseFrame] };

                bool const isValidTimeStamp{ framePoseIndexMap.find(poseFrame) != framePoseIndexMap.end() };
                bool const isMissingPose{ std::find(missingPoses.begin(), missingPoses.end(), jointIndex) != missingPoses.end() };
                if(isValidTimeStamp && !isMissingPose) {
                    return i;
                }
            }
            return std::nullopt;
        }

        std::optional<size_t> getNextIndex(AnimationClip const &clip, float frame, float framesPerSecond, JointIndexType jointIndex, std::map<float, size_t> &framePoseIndexMap, std::map<float, std::vector<JointIndexType>> &missingPoseMap) {
            size_t const currIndex{ framePoseIndexMap[frame] };
            for(int i = currIndex + 1; i < clip.poses.size(); ++i) {
                float const poseFrame{ clip.poses[i].timeStamp * framesPerSecond };//Get the time of the pose after this one to check if it's not a missing pose
                auto const &missingPoses{ missingPoseMap[poseFrame] };

                bool const isValidTimeStamp{ framePoseIndexMap.find(poseFrame) != framePoseIndexMap.end() };
                bool const isMissingPose{ std::find(missingPoses.begin(), missingPoses.end(), jointIndex) != missingPoses.end() };
                if(isValidTimeStamp && !isMissingPose) {
                    return i;
                }
            }

            return std::nullopt;
        }

        //static std::shared_ptr<gfx::Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, const std::shared_ptr<garlic::clove::GraphicsFactory>& graphicsFactory) {
        //	std::shared_ptr<gfx::Texture> texture;

        //	//TODO: Support multiple textures of the same type
        //	if(material->GetTextureCount(type) > 0) {
        //		aiString path;
        //		material->GetTexture(type, 0, &path);

        //		gfx::TextureDescriptor descriptor{};
        //		texture = graphicsFactory->createTexture(descriptor, path.C_Str());
        //	}

        //	return texture;
        //}

        enum class MeshType {
            Default,
            Animated
        };
        std::shared_ptr<Mesh> processMesh(aiMesh *mesh, aiScene const *scene, MeshType const meshType) {
            size_t const vertexCount = mesh->mNumVertices;

            std::vector<Vertex> vertices(vertexCount);
            std::vector<uint16_t> indices;

            //Build the map of jointIds + weights for each vertex
            std::unordered_map<size_t, std::vector<std::pair<JointIndexType, float>>> vertWeightPairs;
            if(meshType == MeshType::Animated) {
                for(JointIndexType i = 0; i < mesh->mNumBones; ++i) {
                    aiBone *bone = mesh->mBones[i];
                    for(size_t j = 0; j < bone->mNumWeights; ++j) {
                        aiVertexWeight const &vertexWeight = bone->mWeights[j];
                        vertWeightPairs[vertexWeight.mVertexId].emplace_back(i, vertexWeight.mWeight);
                    }
                }
            }

            //Build the vertex array
            for(size_t i = 0; i < vertexCount; ++i) {
                if(mesh->HasPositions()) {
                    vertices[i].position.x = mesh->mVertices[i].x;
                    vertices[i].position.y = mesh->mVertices[i].y;
                    vertices[i].position.z = mesh->mVertices[i].z;
                }
                if(mesh->HasNormals()) {
                    vertices[i].normal.x = mesh->mNormals[i].x;
                    vertices[i].normal.y = mesh->mNormals[i].y;
                    vertices[i].normal.z = mesh->mNormals[i].z;
                }
                if(mesh->HasTextureCoords(0)) {
                    vertices[i].texCoord.x = mesh->mTextureCoords[0][i].x;
                    vertices[i].texCoord.y = mesh->mTextureCoords[0][i].y;
                }
                if(mesh->HasVertexColors(0)) {
                    aiColor4D const &colour = mesh->mColors[0][i];
                    vertices[i].colour      = { colour.r, colour.g, colour.b };
                } else {
                    vertices[i].colour = { 1.0f, 1.0f, 1.0f };
                }
                if(meshType == MeshType::Animated) {
                    vec4i &jointIds = vertices[i].jointIds;
                    vec4f &weights  = vertices[i].weights;

                    std::vector<std::pair<JointIndexType, float>> const &weightPairs = vertWeightPairs[i];

                    for(size_t j = 0; j < 4; ++j) {
                        if(j < weightPairs.size()) {
                            jointIds[j] = weightPairs[j].first;
                            weights[j]  = weightPairs[j].second;
                        } else {
                            jointIds[j] = 0;
                            weights[j]  = 0.0f;
                        }
                    }
                }
            }

            //Build the index array
            for(size_t i = 0; i < mesh->mNumFaces; ++i) {
                aiFace face = mesh->mFaces[i];
                for(size_t j = 0; j < face.mNumIndices; ++j) {
                    indices.emplace_back(face.mIndices[j]);
                }
            }

            //Set up the Material for the mesh
            if(mesh->mMaterialIndex >= 0) {
                //TODO
                /*aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			    auto diffuseTexture = loadMaterialTexture(material, aiTextureType_DIFFUSE, graphicsFactory);
			    auto specularTexture = loadMaterialTexture(material, aiTextureType_SPECULAR, graphicsFactory);

			    if(diffuseTexture) {
			    	meshMaterial->setAlbedoTexture(std::move(diffuseTexture));
			    }
			    if(specularTexture) {
			    	meshMaterial->setSpecularTexture(std::move(specularTexture));
			    }*/
            }

            return std::make_shared<Mesh>(std::move(vertices), std::move(indices));
        }

        aiScene const *openFile(std::string_view modelFilePath, Assimp::Importer &importer) {
            return importer.ReadFile(modelFilePath.data(), aiProcess_JoinIdenticalVertices | aiProcess_MakeLeftHanded | aiProcess_Triangulate | aiProcess_FlipUVs);
        }
    }

    StaticModel loadStaticModel(std::string_view modelFilePath) {
        CLOVE_PROFILE_FUNCTION();

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Loading static model: {0}", modelFilePath);

        std::vector<std::shared_ptr<Mesh>> meshes;

        Assimp::Importer importer;
        const aiScene *scene = openFile(modelFilePath.data(), importer);
        if(scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode == nullptr) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Assimp Error: {0}", importer.GetErrorString());
            return { meshes, std::make_shared<Material>() };
        }

        for(size_t i = 0; i < scene->mNumMeshes; ++i) {
            aiMesh *mesh{ scene->mMeshes[i] };
            meshes.emplace_back(processMesh(mesh, scene, MeshType::Default));
        }

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Info, "Finished loading static model: {0}", modelFilePath);
        return { meshes, std::make_shared<Material>() };
    }

    AnimatedModel loadAnimatedModel(std::string_view modelFilePath) {
        CLOVE_PROFILE_FUNCTION();

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "Loading animated model: {0}", modelFilePath);

        std::vector<std::shared_ptr<Mesh>> meshes;

        Assimp::Importer importer;
        const aiScene *scene = openFile(modelFilePath.data(), importer);
        if(scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode == nullptr) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Assimp Error: {0}", importer.GetErrorString());
            return { meshes, std::make_shared<Material>(), nullptr, {} };
        }

        //TODO: Support multiple skeletons?
        std::unique_ptr<Skeleton> skeleton = std::make_unique<Skeleton>();

        //Lookup maps
        std::unordered_map<std::string, aiNode *> nodeNameMap;
        std::unordered_map<std::string, JointIndexType> jointNameIdMap;
        buildNodeNameMap(nodeNameMap, scene->mRootNode);

        //Build skeleton
        bool skeletonSet{ false };
        for(size_t i = 0; i < scene->mNumMeshes; ++i) {
            aiMesh *mesh{ scene->mMeshes[i] };
            meshes.emplace_back(processMesh(mesh, scene, MeshType::Animated));

            if(mesh->mNumBones <= 0 || skeletonSet) {
                continue;
            }

            skeleton->joints.resize(mesh->mNumBones);
            for(size_t i = 0; i < mesh->mNumBones; i++) {
                aiBone *bone{ mesh->mBones[i] };
                skeleton->joints[i].name            = bone->mName.C_Str();
                skeleton->joints[i].inverseBindPose = convertToGarlicMatrix(bone->mOffsetMatrix);

                jointNameIdMap[skeleton->joints[i].name] = i;
            }

            //Only doing one sekelton for now
            skeletonSet = true;
        }

        //Set parents
        for(auto &joint : skeleton->joints) {
            if(auto *aiJoint{ nodeNameMap[joint.name] }; aiJoint->mParent != nullptr) {
                if(jointNameIdMap.contains(aiJoint->mParent->mName.C_Str())) {
                    joint.parentIndex = jointNameIdMap[aiJoint->mParent->mName.C_Str()];
                }
            }
        }

        //Load animations
        std::vector<AnimationClip> animationClips(scene->mNumAnimations);
        for(size_t animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex) {
            aiAnimation *animation  = scene->mAnimations[animIndex];
            AnimationClip &animClip = animationClips[animIndex];

            animClip.skeleton = skeleton.get();
            animClip.duration = animation->mDuration / animation->mTicksPerSecond;//Our clip is in seconds where as the animation is in frames

            //Get all the key frame times for every possible channel
            std::set<float> frames;
            for(size_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
                aiNodeAnim *channel = animation->mChannels[channelIndex];
                for(size_t key = 0; key < channel->mNumPositionKeys; ++key) {
                    frames.emplace(static_cast<float>(channel->mPositionKeys[key].mTime));
                }
                for(size_t key = 0; key < channel->mNumRotationKeys; ++key) {
                    frames.emplace(static_cast<float>(channel->mRotationKeys[key].mTime));
                }
                for(size_t key = 0; key < channel->mNumScalingKeys; ++key) {
                    frames.emplace(static_cast<float>(channel->mScalingKeys[key].mTime));
                }
            }

            std::map<float, std::vector<JointIndexType>> missingPositions;
            std::map<float, std::vector<JointIndexType>> missingRotations;
            std::map<float, std::vector<JointIndexType>> missingScale;

            std::map<float, size_t> framePoseIndexMap;

            //Get each channel's pose at each time
            for(float frame : frames) {
                AnimationPose animPose{};
                animPose.timeStamp = frame / animation->mTicksPerSecond;
                animPose.poses.resize(std::size(skeleton->joints));

                for(size_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
                    aiNodeAnim *channel = animation->mChannels[channelIndex];

                    //TODO: The 'Armature' represents the root motion (I think). If so then this should contribute to the model and not the animation
                    if(strcmp(channel->mNodeName.C_Str(), "Armature") == 0) {
                        continue;
                    }

                    //Assimp appears to have trouble importing FBX animations. So we need to see if it's added nodes and then map them to the correct ones.
                    std::string const nodeName{ channel->mNodeName.C_Str() };
                    if(!jointNameIdMap.contains(nodeName)){
                        std::string_view constexpr assimpFbx{ "_$AssimpFbx$_" };

                        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "{0} not found in skeleton, searching for {1}...", nodeName, assimpFbx);
                        size_t const pos{ nodeName.find(assimpFbx) };

                        if(pos != nodeName.npos) {
                            std::string const jointName{ nodeName.substr(0, pos) };
                            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Trace, "{0} substr found! Using index for {1}", assimpFbx, jointName);
                            jointNameIdMap[nodeName] = jointNameIdMap[jointName];
                        } else {
                            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "Could not find {0} in skeleton or a substr of {1} in {0}.", nodeName, assimpFbx);
                            continue;
                        }
                    }

                    JointIndexType const jointIndex{ jointNameIdMap[nodeName] };
                    JointPose &jointPose{ animPose.poses[jointIndex] };

                    bool positionFound{ false };
                    bool rotationFound{ false };
                    bool scaleFound{ false };

                    for(size_t key = 0; key < channel->mNumPositionKeys; ++key) {
                        if(channel->mPositionKeys[key].mTime == frame) {
                            jointPose.position = convertToGarlicVec(channel->mPositionKeys[key].mValue);
                            positionFound      = true;
                            break;
                        }
                    }
                    for(size_t key = 0; key < channel->mNumRotationKeys; ++key) {
                        if(channel->mRotationKeys[key].mTime == frame) {
                            jointPose.rotation = convertToGarlicQuat(channel->mRotationKeys[key].mValue);
                            rotationFound      = true;
                            break;
                        }
                    }
                    for(size_t key = 0; key < channel->mNumScalingKeys; ++key) {
                        if(channel->mScalingKeys[key].mTime == frame) {
                            jointPose.scale = convertToGarlicVec(channel->mScalingKeys[key].mValue);
                            scaleFound      = true;
                            break;
                        }
                    }

                    //Because the Animator expects every AnimationPose to have a pose for every joint
                    //we need to keep track of all the missing elements so we can interpolate between them later
                    //TODO: To simplify this we can refactor the animator to handle a pose not having an entry for every joints
                    if(!positionFound) {
                        missingPositions[frame].push_back(jointIndex);
                    }
                    if(!rotationFound) {
                        missingRotations[frame].push_back(jointIndex);
                    }
                    if(!scaleFound) {
                        missingScale[frame].push_back(jointIndex);
                    }
                }

                animClip.poses.emplace_back(std::move(animPose));
                framePoseIndexMap[frame] = std::size(animClip.poses) - 1;
            }

            auto const retrieveJointPoses = [&](float time, JointIndexType jointIndex, AnimationPose const &currAnimPose, std::map<float, std::vector<JointIndexType>> &missingElementMap) {
                struct LerpData {
                    bool isValid{ false };
                    float const lerpTime;
                    JointPose const *prevPose;
                    JointPose const *nextPose;
                };

                std::optional<size_t> const prevPoseIndex{ getPreviousIndex(animClip, time, animation->mTicksPerSecond, jointIndex, framePoseIndexMap, missingElementMap) };
                std::optional<size_t> const nextPoseIndex{ getNextIndex(animClip, time, animation->mTicksPerSecond, jointIndex, framePoseIndexMap, missingElementMap) };

                if(!prevPoseIndex || !nextPoseIndex) {
                    return LerpData{ false };
                }

                AnimationPose const &prevAnimPose{ animClip.poses[*prevPoseIndex] };
                AnimationPose const &nextAnimPose{ animClip.poses[*nextPoseIndex] };

                float const timeBetweenPoses{ nextAnimPose.timeStamp - prevAnimPose.timeStamp };
                float const timeFromPrevPose{ currAnimPose.timeStamp - prevAnimPose.timeStamp };
                float const normTime{ timeFromPrevPose / timeBetweenPoses };

                return LerpData{
                    .lerpTime = normTime,
                    .prevPose = &prevAnimPose.poses[jointIndex],
                    .nextPose = &nextAnimPose.poses[jointIndex],
                };
            };

            //Interpolate missing keyframes
            for(auto &[time, jointIndices] : missingPositions) {
                size_t const currPoseIndex{ framePoseIndexMap[time] };
                AnimationPose &currAnimPose{ animClip.poses[currPoseIndex] };

                for(JointIndexType jointIndex : jointIndices) {
                    auto const lerpData{ retrieveJointPoses(time, jointIndex, currAnimPose, missingPositions) };

                    if(!lerpData.isValid){
                        continue;
                    }

                    JointPose &pose{ currAnimPose.poses[jointIndex] };
                    pose.position = lerp(lerpData.prevPose->position, lerpData.nextPose->position, lerpData.lerpTime);
                }
            }
            for(auto &[time, jointIndices] : missingRotations) {
                size_t const currPoseIndex{ framePoseIndexMap[time] };
                AnimationPose &currAnimPose{ animClip.poses[currPoseIndex] };

                for(JointIndexType jointIndex : jointIndices) {
                    auto const lerpData{ retrieveJointPoses(time, jointIndex, currAnimPose, missingRotations) };

                    if(!lerpData.isValid) {
                        continue;
                    }

                    JointPose &pose{ currAnimPose.poses[jointIndex] };
                    pose.rotation = slerp(lerpData.prevPose->rotation, lerpData.nextPose->rotation, lerpData.lerpTime);
                }
            }
            for(auto &[time, jointIndices] : missingScale) {
                size_t const currPoseIndex{ framePoseIndexMap[time] };
                AnimationPose &currAnimPose{ animClip.poses[currPoseIndex] };

                for(JointIndexType jointIndex : jointIndices) {
                    auto const lerpData{ retrieveJointPoses(time, jointIndex, currAnimPose, missingScale) };

                    if(!lerpData.isValid) {
                        continue;
                    }

                    JointPose &pose{ currAnimPose.poses[jointIndex] };
                    pose.scale = lerp(lerpData.prevPose->scale, lerpData.nextPose->scale, lerpData.lerpTime);
                }
            }
        }

        CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Info, "Finished loading animated model: {0}", modelFilePath);
        return { meshes, std::make_shared<Material>(), std::move(skeleton), std::move(animationClips) };
    }
}