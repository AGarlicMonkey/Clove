#version 450

//TODO: Push constant
layout(std140, set = 0, binding = 2) uniform Light{
    vec3 lightPos;
    float lightRadius;
};

layout(location = 0) in vec4 fragPos;

void main(){
    gl_FragDepth = length(fragPos.xyz - lightPos) / lightRadius;
}