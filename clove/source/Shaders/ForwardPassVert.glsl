#version 450

#include "Constants.glsl"

layout(std140, set = 0, binding = 0) uniform Model{
	mat4 model;
	mat4 normalMatrix;
};

layout(std140, set = 0, binding = 1) uniform ViewProj{
	mat4 view;
	mat4 proj;
};

layout(std140, set = 0, binding = 2) uniform LightMatrix{
	mat4 directionalLightSpaceMatrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 vertPos;
layout(location = 2) out vec3 vertNorm;
layout(location = 3) out vec4 vertPosLightSpace;

void main(){
	const vec4 worldPos = model * vec4(position, 1.0f);

	gl_Position = proj * view * worldPos;
	
	fragTexCoord = texCoord;

	vertPos			  = worldPos.xyz;
	vertNorm		  = mat3(normalMatrix) * normal;
	vertPosLightSpace = directionalLightSpaceMatrix * vec4(vertPos, 1.0f);
}