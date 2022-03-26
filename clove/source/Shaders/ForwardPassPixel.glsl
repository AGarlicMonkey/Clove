#version 450

#include "Common.glsl"
#include "Constants.glsl"

#define SCREEN_GRID_SIZE 16

layout(set = 0, binding = 4) uniform texture2D diffuseTexture;
layout(set = 0, binding = 5) uniform texture2D specularTexture;
layout(set = 0, binding = 6) uniform sampler meshSampler;

layout(set = 0, binding = 7) uniform texture2D directionalDepthTexture;
layout(set = 0, binding = 8) uniform textureCubeArray pointLightDepthTexture;
layout(Set = 0, binding = 9) uniform sampler shadowSampler;

layout(set = 0, binding = 10) uniform ViewPosition{
	vec3 viewPos;
};

layout(set = 0, binding = 13) uniform utexture2D lightGrid;
layout(set = 0, binding = 15) uniform sampler lightGridSampler;

layout(std140, set = 0, binding = 14) buffer readonly CulledLights{
	uint lightIndexList[];
};

layout(std140, set = 0, binding = 11) buffer readonly Lights{
	Light lights[];
};

layout(std140, set = 0, binding = 12) uniform Colour{
	vec4 colour;
};

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 fragNorm;
layout(location = 3) in vec4 fragPosLightSpace;

layout(location = 0) out vec4 outColour;

struct MaterialInput{
	vec3 diffuseColour;
	vec3 specularColour;

	float shininess;

	vec3 normal;
};

struct LightResult{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shadow;
};

//Adjusts the bias to be in between min and max based off of the angle to the light
float adjustBias(vec3 normal, vec3 lightDir){
	const float minBias = 0.0005f;
	const float maxBias = 0.025f;

	return max(maxBias * (1.0f - dot(normal, lightDir)), minBias);
}

LightResult getDirectionalLighting(Light light, MaterialInput materialInput, vec3 viewDir){
	LightResult result;

	const vec3 lightDir = normalize(-light.direction); //Gets the direction towards the light

	//Ambient
	result.ambient = materialInput.diffuseColour * light.ambient;
	
	//Diffuse
	const float diffIntensity = max(dot(materialInput.normal, lightDir), 0.0f);
	result.diffuse = materialInput.diffuseColour * light.diffuse * diffIntensity;

	//Specular
	const vec3 reflectDir = reflect(-lightDir, materialInput.normal);
	const float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0f), materialInput.shininess);
	result.specular = materialInput.specularColour * light.specular * specIntensity;

	//Shadow
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xy = projCoords.xy * 0.5f + 0.5f;

	const float currentDepth = projCoords.z;
	const float closestDepth = texture(sampler2D(directionalDepthTexture, shadowSampler), projCoords.xy).r;

	const float depthBias = adjustBias(materialInput.normal, lightDir);
	result.shadow = currentDepth - depthBias > closestDepth ? 1.0f : 0.0f;

	return result;
}

LightResult getPointLighting(Light light, MaterialInput materialInput, vec3 viewDir){
	LightResult result;

	const vec3 lightDir = normalize(light.position - fragPos);

	//Ambient
	result.ambient = materialInput.diffuseColour * light.ambient;

	//Diffuse
	const float diffIntensity  = max(dot(materialInput.normal, lightDir), 0.0f);
	result.diffuse 			   = materialInput.diffuseColour * light.diffuse * diffIntensity;

	//Specular
	const vec3 reflectDir	  = reflect(-lightDir, materialInput.normal);
	const float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0f), materialInput.shininess);
	result.specular			  = materialInput.specularColour * light.specular * specIntensity;

	//Attenuation
	const float dist 		= length(light.position - fragPos);
	const float attenuation = max((1.0f / sqrt(dist)) - (1.0f / sqrt(light.radius)), 0.0f);

	result.ambient *= attenuation;
	result.diffuse *= attenuation;
	result.specular	*= attenuation;

	//Shadow
	const vec3 lightToFrag = fragPos - light.position;

	const float sampledDepth = texture(samplerCubeArray(pointLightDepthTexture, shadowSampler), vec4(lightToFrag, light.shadowIndex)).r;

	const float currentDepth = length(lightToFrag);
	const float closestDepth = sampledDepth * light.radius;

	const float depthBias = adjustBias(materialInput.normal, lightDir);
	result.shadow = currentDepth - depthBias > closestDepth ? 1.0f : 0.0f;

	return result;
}

void main(){
	MaterialInput materialInput;
	materialInput.diffuseColour  = texture(sampler2D(diffuseTexture, meshSampler), fragTexCoord).rgb;
	materialInput.specularColour = texture(sampler2D(specularTexture, meshSampler), fragTexCoord).rgb;
	materialInput.shininess = 32.0f; //TODO: Add shininess as a material param
	materialInput.normal  = normalize(fragNorm);

	const vec3 viewDir = normalize(viewPos - fragPos);

	const ivec2 tileIndex 	= ivec2(floor(gl_FragCoord / SCREEN_GRID_SIZE));
	const uvec2 tileData	= texelFetch(usampler2D(lightGrid, lightGridSampler), tileIndex, 0).xy;
	const uint startOffset 	= tileData.x;
	const uint lightCount 	= tileData.y;

	//Compute lighting
	vec3 lighting = vec3(0.0f);

	for(int i = 0; i < lightCount; ++i) {
		const Light light = lights[lightIndexList[startOffset + i]];
		LightResult result;

		switch(light.type) {
			case LIGHT_TYPE_DIRECTIONAL:
				result = getDirectionalLighting(light, materialInput, viewDir);
				break;
			case LIGHT_TYPE_POINT:
				result = getPointLighting(light, materialInput, viewDir);
				break;
		}

		lighting += (result.ambient + ((1.0f - result.shadow) * (result.diffuse + result.specular)));
	}

	outColour = vec4(lighting, 1.0f) * colour;
}