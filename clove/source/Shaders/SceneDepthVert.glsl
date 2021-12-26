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

layout(location = 0) in vec3 position;

void main(){
    gl_Position = proj * view * model * vec4(position, 1.0f);
}