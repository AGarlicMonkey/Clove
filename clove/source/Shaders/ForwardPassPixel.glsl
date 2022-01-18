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

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 fragNorm;
layout(location = 4) in vec4 fragPosLightSpaces;

layout(location = 0) out vec4 outColour;

struct MaterialInput{
	vec3 diffuseColour;
	vec3 specularColour;

	float shininess;

	vec3 viewDir;
	vec3 normal;
};

//Adjusts the bias to be in between min and max based off of the angle to the light
float adjustBias(float minBias, float maxBias, vec3 normal, vec3 lightDir){
	return max(maxBias * (1.0f - dot(normal, lightDir)), minBias);
}

void getDirectionalLighting(Light light, MaterialInput materialInput, inout vec3 outAmbient, inout vec3 outDiffuse, inout vec3 outSpecular, inout float outShadow){
	const float minBias = 0.0005f;
	const float maxBias = 0.025f;

	const vec3 lightDir = normalize(-light.direction); //Gets the direction towards the light

	//Ambient
	outAmbient += materialInput.diffuseColour * light.ambient;
	
	//Diffuse
	const float diffIntensity = max(dot(materialInput.normal, lightDir), 0.0f);
	outDiffuse += materialInput.diffuseColour * light.diffuse * diffIntensity;

	//Specular
	const vec3 reflectDir = reflect(-lightDir, materialInput.normal);
	const float specIntensity = pow(max(dot(materialInput.viewDir, reflectDir), 0.0f), materialInput.shininess);
	outSpecular += materialInput.specularColour * light.specular * specIntensity;

	//Shadow
	vec3 projCoords = fragPosLightSpaces.xyz / fragPosLightSpaces.w;
	projCoords.xy = projCoords.xy * 0.5f + 0.5f;

	const float currentDepth = projCoords.z;
	const float closetDepth  = texture(sampler2D(directionalDepthTexture, shadowSampler), projCoords.xy).r;

	outShadow += currentDepth - adjustBias(minBias, maxBias, materialInput.normal, lightDir) > closetDepth ? 1.0f : 0.0f;
}

void getPointLighting(Light light, MaterialInput materialInput, inout vec3 outAmbient, inout vec3 outDiffuse, inout vec3 outSpecular, in out float outShadow){
	const float minBias = 0.0005f;
	const float maxBias = 0.025f;

	const vec3 lightDir = normalize(light.position - fragPos);

	//Ambient
	vec3 ambient = materialInput.diffuseColour * light.ambient;

	//Diffuse
	const float diffIntensity = max(dot(materialInput.normal, lightDir), 0.0f);
	vec3 diffuse = materialInput.diffuseColour * light.diffuse * diffIntensity;

	//Specular
	const vec3 reflectDir = reflect(-lightDir, materialInput.normal);
	const float specIntensity = pow(max(dot(materialInput.viewDir, reflectDir), 0.0f), materialInput.shininess);
	vec3 specular = materialInput.specularColour * light.specular * specIntensity;

	//Attenuation
	const float dist 		= length(light.position - fragPos);
	const float attenuation = 1.0f / (light.constant + (light.linearV * dist) + (light.quadratic * (dist * dist)));

	ambient		*= attenuation;
	diffuse		*= attenuation;
	specular	*= attenuation;

	outAmbient += ambient;
	outDiffuse += diffuse;
	outSpecular += specular;

	//Shadow
	const vec3 lightToFrag = fragPos - light.position;

	const float currentDepth = length(lightToFrag);
	const float closetDepth  = texture(samplerCubeArray(pointLightDepthTexture, shadowSampler), vec4(lightToFrag, light.shadowIndex)).r * light.radius;

	outShadow += currentDepth - adjustBias(minBias, maxBias, materialInput.normal, lightDir) > closetDepth ? 1.0f : 0.0f;
}

void main(){
	MaterialInput materialInput;
	materialInput.diffuseColour  = texture(sampler2D(diffuseTexture, meshSampler), fragTexCoord).rgb;
	materialInput.specularColour = texture(sampler2D(specularTexture, meshSampler), fragTexCoord).rgb;
	
	materialInput.shininess = 32.0f; //TODO: Add shininess as a material param

	materialInput.viewDir = normalize(viewPos - fragPos);
	materialInput.normal  = normalize(fragNorm);

	vec3 totalAmbient 	= vec3(0.0f);
	vec3 totalDiffuse 	= vec3(0.0f);
	vec3 totalSpecular 	= vec3(0.0f);

	float shadow = 0.0f;

	const ivec2 tileIndex 	= ivec2(floor(gl_FragCoord / SCREEN_GRID_SIZE));
	const uvec2 tileData	= texelFetch(usampler2D(lightGrid, lightGridSampler), tileIndex, 0).xy;
	const uint startOffset 	= tileData.x;
	const uint lightCount 	= tileData.y;

	//Compute lighting
	for(int i = 0; i < lightCount; ++i) {
		const Light light = lights[startOffset + i];

		switch(light.type) {
			case LIGHT_TYPE_DIRECTIONAL:
				getDirectionalLighting(light, materialInput, totalAmbient, totalDiffuse, totalSpecular, shadow);
				break;
			case LIGHT_TYPE_POINT:
				getPointLighting(light, materialInput, totalAmbient, totalDiffuse, totalSpecular, shadow);
				break;
		}
	}

	shadow /= (lightCount);

	const vec3 lighting = (totalAmbient + ((1.0f - shadow) * (totalDiffuse + totalSpecular)));

	outColour = vec4(lighting, 1.0f) * colour;
}