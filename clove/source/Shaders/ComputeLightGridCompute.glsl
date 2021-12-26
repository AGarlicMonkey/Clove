#version 450

#include "Constants.glsl"
#include "Common.glsl"

#define MAX_LIGHTS 1024
#define MAX_LIGHTS_PER_TILE 256
#define GROUP_SIZE 16

layout(set = 0, binding = 0) uniform writeonly image2D lightGrid;
layout(set = 0, binding = 1, r32f) uniform readonly image2D sceneDepth;

layout(std140, set = 0, binding = 2) buffer CulledLights{
	uint lightIndexList[];
};

layout(std140, set = 0, binding = 3) buffer LightCounter{
	uint lightCounter;
};

layout(std140, set = 0, binding = 4) uniform FrustumData{
	vec2 screenDimensions;
	mat4 inverseProjection;
};

//TODO: push constant
layout(std140, set = 0, binding = 5) uniform NumLights{
	uint numLights;
};

layout(std140, set = 0, binding = 6) buffer readonly Lights{
	Light lights[];
};

shared uint minDepth;
shared uint maxDepth;

shared uint lightCount;
shared uint lightIndexStartOffset;
shared uint lightList[MAX_LIGHTS_PER_TILE];

layout(local_size_x = GROUP_SIZE) in;
layout(local_size_y = GROUP_SIZE) in;

struct Plane {
	vec3 normal;
	float dist;
};

struct Frustum {
	Plane planes[6];
};

Plane computePlane(vec3 p0, vec3 p1, vec3 p2) {
	Plane plane;

	const vec3 v0 = p1 - p0;
	const vec3 v1 = p2 - p0;

	plane.normal = normalize(cross(v0, v1));
	plane.dist	 = dot(plane.normal, p0);

	return plane;
}

vec4 clipToView(vec4 clip) {
	vec4 view = inverseProjection *clip;
    view = view / view.w;

    return view;
}

vec4 screenToView(vec4 screen) {
    const vec2 texCoord = screen.xy / screenDimensions;
    const vec4 clip = vec4(vec2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

    return clipToView(clip);
}

bool isLightInsideFrustum(Light light, Frustum frustum) {
	switch(light.type) {
		case LIGHT_TYPE_DIRECTIONAL:
			return true; //Directional lights will always pass as they take up the entire view
		case LIGHT_TYPE_POINT: {
			bool isInside = true;
			for(int i = 0; i < 6 && isInside; ++i){
				const Plane plane = frustum.planes[i];
				isInside = dot(plane.normal, light.position) - plane.dist < -light.radius;
			}
			return isInside;
		}
	}
}

void main(){
	const ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);

	if(gl_LocalInvocationIndex == 0) {
		minDepth = 0xFFFFFFFF;
		maxDepth = 0;

		lightCount = 0;
	}

	groupMemoryBarrier();

	{
		const uint depth = floatBitsToUint(imageLoad(sceneDepth, pixelPos).r);

		atomicMin(minDepth, depth);
		atomicMax(maxDepth, depth);
	}
	
	groupMemoryBarrier();

	//Compute frustum - TODO: Only needs to be calculated if screen changes size (min and max will always need to be calculated)
    Frustum frustum;
	{
		const vec3 eyePos = vec3(0.0f);

		vec4 screenSpace[4];
    	screenSpace[0] = vec4(pixelPos.xy * GROUP_SIZE, -1.0f, 1.0f); 								
    	screenSpace[1] = vec4(vec2(pixelPos.x + 1, 	pixelPos.y) 		* GROUP_SIZE, -1.0f, 1.0f); 
    	screenSpace[2] = vec4(vec2(pixelPos.x, 		pixelPos.y + 1) 	* GROUP_SIZE, -1.0f, 1.0f); 
    	screenSpace[3] = vec4(vec2(pixelPos.x + 1, 	pixelPos.y + 1) 	* GROUP_SIZE, -1.0f, 1.0f); 

		vec3 viewSpace[4];
		for(int i = 0; i < 4; ++i){
			viewSpace[i] = screenToView(screenSpace[i]).xyz;
		}

		const float minDepthVs = clipToView(vec4(0.0f, 0.0f, uintBitsToFloat(minDepth), 1.0f)).z;
		const float maxDepthVs = clipToView(vec4(0.0f, 0.0f, uintBitsToFloat(maxDepth), 1.0f)).z;

    	frustum.planes[0] = computePlane(eyePos, viewSpace[2], viewSpace[0]); 	//Top left
    	frustum.planes[1] = computePlane(eyePos, viewSpace[1], viewSpace[3]); 	//Top right
    	frustum.planes[2] = computePlane(eyePos, viewSpace[0], viewSpace[1]); 	//Bottom left
    	frustum.planes[3] = computePlane(eyePos, viewSpace[3], viewSpace[2]); 	//Bottom right
		frustum.planes[4] = Plane(vec3(0.0f, 0.0f, -1.0f), -minDepthVs); 		//Near
		frustum.planes[5] = Plane(vec3(0.0f, 0.0f,  1.0f),  maxDepthVs);		//Far
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