#version 450

#include "Constants.glsl"

//TODO: Push constant
layout(std140, set = 0, binding = 1) uniform Colour{
    vec4 colour;
};

layout(set = 0, binding = 2) uniform texture2D widgetTexture;
layout(set = 0, binding = 3) uniform sampler texSampler;

layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColour;

void main(){
    outColour = texture(sampler2D(widgetTexture, texSampler), texCoord) * colour;
}
