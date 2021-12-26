#version 450

#include "Common.glsl"
#include "Constants.glsl"

#define SCREEN_GRID_SIZE 16

layout(set = 0, binding = 4) uniform texture2D diffuseTexture;
layout(set = 0, binding = 5) uniform texture2D specularTexture;
layout(set = 0, binding = 6) uniform sampler meshSampler;

layout(set = 0, binding = 7) uniform texture2DArray directionalDepthTexture;
layout(set = 0, binding = 8) uniform textureCubeArray pointLightDepthTexture;
layout(Set = 0, binding = 9) uniform sampler shadowSampler;

layout(set = 0, binding = 10) uniform ViewPosition{
	vec3 viewPos;
};

layout(set = 0, binding = 13, rg32ui) uniform readonly uimage2D lightGrid;

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

struct LightingInput{
	Light light;

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

void getDirectionalLighting(LightingInput lightingInput, inout vec3 outAmbient, inout vec3 outDiffuse, inout vec3 outSpecular, inout float outShadow){
	const float minBias = 0.0005f;
	const float maxBias = 0.025f;

	const Light light = lightingInput.light;

	const vec3 lightDir = normalize(-light.direction); //Gets the direction towards the light
		
	//Ambient
	outAmbient += lightingInput.diffuseColour * light.ambient;
	
	//Diffuse
	const float diffIntensity = max(dot(lightingInput.normal, lightDir), 0.0f);
	outDiffuse += lightingInput.diffuseColour * light.diffuse * diffIntensity;

	//Specular
	const vec3 reflectDir = reflect(-lightDir, lightingInput.normal);
	const float specIntensity = pow(max(dot(lightingInput.viewDir, reflectDir), 0.0f), lightingInput.shininess);
	outSpecular += lightingInput.specularColour * light.specular * specIntensity;

	//Shadow
	vec3 projCoords = fragPosLightSpaces[light.shadowIndex].xyz / fragPosLightSpaces[light.shadowIndex].w;
	projCoords.xy = projCoords.xy * 0.5f + 0.5f;

	const float currentDepth = projCoords.z;
	const float closetDepth  = texture(sampler2DArray(directionalDepthTexture, shadowSampler), vec3(projCoords.xy, light.shadowIndex)).r;

	outShadow += currentDepth - adjustBias(minBias, maxBias, lightingInput.normal, lightDir) > closetDepth ? 1.0f : 0.0f;
}

void getPointLighting(LightingInput lightingInput, inout vec3 outAmbient, inout vec3 outDiffuse, inout vec3 outSpecular, in out float outShadow){
	const float minBias = 0.0005f;
	const float maxBias = 0.025f;

	const Light light = lightingInput.light;

	const vec3 lightDir = normalize(light.position - fragPos);

	//Ambient
	vec3 ambient = lightingInput.diffuseColour * light.ambient;

	//Diffuse
	const float diffIntensity = max(dot(lightingInput.normal, lightDir), 0.0f);
	vec3 diffuse = lightingInput.diffuseColour * light.diffuse * diffIntensity;

	//Specular
	const vec3 reflectDir = reflect(-lightDir, lightingInput.normal);
	const float specIntensity = pow(max(dot(lightingInput.viewDir, reflectDir), 0.0f), lightingInput.shininess);
	vec3 specular = lightingInput.specularColour * light.specular * specIntensity;

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

	outShadow += currentDepth - adjustBias(minBias, maxBias, lightingInput.normal, lightDir) > closetDepth ? 1.0f : 0.0f;
}

void main(){
	LightingInput lightingInput;
	lightingInput.diffuseColour 	= texture(sampler2D(diffuseTexture, meshSampler), fragTexCoord).rgb;
	lightingInput.specularColour 	= texture(sampler2D(specularTexture, meshSampler), fragTexCoord).rgb;
	
	lightingInput.shininess = 32.0f; //TODO: Add shininess as a material param

	lightingInput.viewDir = normalize(viewPos - fragPos);
	lightingInput.normal = normalize(fragNorm);

	vec3 totalAmbient 	= vec3(0.0f);
	vec3 totalDiffuse 	= vec3(0.0f);
	vec3 totalSpecular 	= vec3(0.0f);

	float shadow = 0.0f;

	const ivec2 tileIndex 	= ivec2(floor(gl_FragCoord / SCREEN_GRID_SIZE));
	const uvec2 tileData	= imageLoad(lightGrid, tileIndex).xy;
	const uint startOffset 	= tileData.x;
	const uint lightCount 	= tileData.y;

	//Compute lighting
	for(int i = 0; i < lightCount; ++i) {
		lightingInput.light = lights[startOffset + i];

		switch(lightingInput.light.type) {
			case LIGHT_TYPE_DIRECTIONAL:
				getDirectionalLighting(lightingInput, totalAmbient, totalDiffuse, totalSpecular, shadow);
				break;
			case LIGHT_TYPE_POINT:
				getPointLighting(lightingInput, totalAmbient, totalDiffuse, totalSpecular, shadow);
				break;
		}
	}

	shadow /= (lightCount);

	const vec3 lighting = (totalAmbient + ((1.0f - shadow) * (totalDiffuse + totalSpecular)));

	outColour = vec4(lighting, 1.0f) * colour;
}