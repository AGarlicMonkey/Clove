#version 450

#include "Constants.glsl"

struct Vertex{
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec3 colour;
    ivec4 jointIds;
    vec4 weights;
};

layout(std140, set = 0, binding = 0) uniform Skeleton{
	mat4 matrixPalet[MAX_JOINTS];
};

//TODO: Make this a push constant
layout(std140, set = 0, binding = 1) uniform VertCount{
	uint vertCount;
};

layout(std140, set = 0, binding = 2) readonly buffer BindVertices{
	Vertex bindVertices[];
};

layout(std140, set = 0, binding = 3) buffer SkinnedVertices{
	Vertex skinnedVertices[];
};

layout(local_size_x = 256) in;

void main(){
    vec4 animatedPos = vec4(0.0f);
    vec3 animatedNormal = vec3(0.0f);

	if(gl_GlobalInvocationID.x < vertCount){
        Vertex bindVertex = bindVertices[gl_GlobalInvocationID.x];
        for(int i = 0; i < 4; ++i){ //4 is the max amount of weights
            const mat4 jointTransform = matrixPalet[bindVertex.jointIds[i]];
            const float weight = bindVertex.weights[i];

            animatedPos += (jointTransform * vec4(bindVertex.position, 1.0f)) * weight;
            animatedNormal += (mat3(jointTransform) * bindVertex.normal) * weight;
        }
    
	    bindVertex.position = animatedPos.xyz / animatedPos.w; //Make sure to compensate for the weighted w. If this division is too much then pos can just be a vec4
        bindVertex.normal = animatedNormal.xyz;

        skinnedVertices[gl_GlobalInvocationID.x] = bindVertex;
	}
}
