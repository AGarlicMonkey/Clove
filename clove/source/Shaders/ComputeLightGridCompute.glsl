#version 450

#include "Constants.glsl"
#include "Common.glsl"

#define MAX_LIGHTS_PER_TILE 256
#define GROUP_SIZE 16
#define NUM_FRUSTUM_PLANES 4

layout(set = 0, binding = 0, rg32ui) uniform writeonly uimage2D lightGrid;
layout(set = 0, binding = 1) uniform texture2D sceneDepth;

layout(std140, set = 0, binding = 2) buffer CulledLights{
	uint lightIndexList[];
};

layout(std140, set = 0, binding = 3) buffer LightCounter{
	uint lightCounter;
};

layout(std140, set = 0, binding = 4) uniform FrustumData{
	uvec2 screenDimensions;
	mat4 projection;
	mat4 view;
};

//TODO: push constant
layout(std140, set = 0, binding = 5) uniform NumLights{
	uint numLights;
};

layout(std140, set = 0, binding = 6) buffer readonly Lights{
	Light lights[];
};

layout(set = 0, binding = 7) uniform sampler sceneDepthSampler;

shared uint minDepth;
shared uint maxDepth;

shared uint lightCount;
shared uint lightIndexStartOffset;
shared uint lightList[MAX_LIGHTS_PER_TILE];

layout(local_size_x = GROUP_SIZE) in;
layout(local_size_y = GROUP_SIZE) in;

struct Frustum {
	vec4 planes[NUM_FRUSTUM_PLANES];
};

bool isLightInsideFrustum(Light light, Frustum frustum) {
	switch(light.type) {
		case LIGHT_TYPE_DIRECTIONAL:
			return true; //Directional lights will always pass as they take up the entire view
		case LIGHT_TYPE_POINT: {
			bool isInside = true;
			for(int i = 0; i < NUM_FRUSTUM_PLANES && isInside; ++i){
				const vec4 plane 		= frustum.planes[i];
				const vec4 lightViewPos = view * vec4(light.position, 1.0f);

				const float distFromPlane = dot(plane, lightViewPos);
				isInside = distFromPlane >= -light.radius;
			}
			return isInside;
		}
	}
}

void main(){
	if(gl_GlobalInvocationID.xy == uvec2(0)){
		//Make sure this buffer does not contain data from the previous frame
		lightCounter = 0;
	}

	if(gl_LocalInvocationIndex == 0) {
		minDepth = 0x7F7FFFFF;
		maxDepth = 0;

		lightCount = 0;
	}

	groupMemoryBarrier();

	{
		const ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
		const uint depth = floatBitsToUint(texelFetch(sampler2D(sceneDepth, sceneDepthSampler), pixelPos, 0).r);

		atomicMin(minDepth, depth);
		atomicMax(maxDepth, depth);
	}
	
	groupMemoryBarrier();

	//Compute frustum - TODO: Only needs to be calculated if screen changes size (min and max will always need to be calculated)
    Frustum frustum;
	{
		const vec2 centerGroup = vec2(screenDimensions.xy) / float(2 * GROUP_SIZE);
    	const vec2 groupOffset = centerGroup - gl_WorkGroupID.xy;

		const vec4 column0 = vec4(projection[0][0] * centerGroup.x,	 0.0f,								projection[2][0] * centerGroup.x + groupOffset.x, 	0.0f);
    	const vec4 column1 = vec4(0.0f,								-projection[1][1] * centerGroup.y, 	projection[2][1] * centerGroup.x + groupOffset.y, 	0.0f);
    	const vec4 column3 = vec4(0.0f,				 	 			 0.0f,								1.0f, 												0.0f);

    	frustum.planes[0] = column3 + column0; //Left
    	frustum.planes[1] = column3 - column0; //Right
    	frustum.planes[2] = column3 - column1; //Top
    	frustum.planes[3] = column3 + column1; //Bottom

		for (int i = 0; i < NUM_FRUSTUM_PLANES; ++i) {
        	frustum.planes[i] /= length(frustum.planes[i].xyz);
    	}
	}

	groupMemoryBarrier();

	//Cull point lights against frustum
	for(uint lightIndex = gl_LocalInvocationIndex; lightIndex < numLights; lightIndex += GROUP_SIZE * GROUP_SIZE) {
		if(isLightInsideFrustum(lights[lightIndex], frustum)){
			const uint index = atomicAdd(lightCount, 1);
			lightList[index] = lightIndex;
		}
	}

	groupMemoryBarrier();

	//Write culled lights into the grid / light list
	if(gl_LocalInvocationIndex == 0){
		lightIndexStartOffset = atomicAdd(lightCounter, lightCount);
		imageStore(lightGrid, ivec2(gl_WorkGroupID.xy), uvec4(lightIndexStartOffset, lightCount, 0, 0));
	}

	groupMemoryBarrier();

	for(uint lightIndex = gl_LocalInvocationIndex; lightIndex < lightCount; lightIndex += GROUP_SIZE * GROUP_SIZE) {
		lightIndexList[lightIndexStartOffset + lightIndex] = lightList[lightIndex];
	}	
}