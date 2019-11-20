#version 460 core

in vec2 vertTexCoord;

layout(binding = 1) uniform sampler2D frameBufferSampler;

layout(location = 0) out vec4 fragmentColour;

const float offset = 1.0 / 300.0;  

void main(){
	fragmentColour = texture(frameBufferSampler, vertTexCoord);
	//fragmentColour = vec4(vec3(1.0 - texture(frameBufferSampler, vertTexCoord)), 1.0);

//	fragmentColour = texture(frameBufferSampler, vertTexCoord);
//    float average = 0.2126 * fragmentColour.r + 0.7152 * fragmentColour.g + 0.0722 * fragmentColour.b;
//    fragmentColour = vec4(average, average, average, 1.0);

//	vec2 offsets[9] = vec2[](
//        vec2(-offset,  offset),  top-left
//        vec2( 0.0f,    offset),  top-center
//        vec2( offset,  offset),  top-right
//        vec2(-offset,  0.0f),    center-left
//        vec2( 0.0f,    0.0f),    center-center
//        vec2( offset,  0.0f),    center-right
//        vec2(-offset, -offset),  bottom-left
//        vec2( 0.0f,   -offset),  bottom-center
//        vec2( offset, -offset)   bottom-right    
//    );
//
//    float kernel[9] = float[](
//        1.0 / 16, 2.0 / 16, 1.0 / 16,
//		2.0 / 16, 4.0 / 16, 2.0 / 16,
//		1.0 / 16, 2.0 / 16, 1.0 / 16  
//    );
//    
//    vec3 sampleTex[9];
//    for(int i = 0; i < 9; i++)
//    {
//        sampleTex[i] = vec3(texture(frameBufferSampler, vertTexCoord.st + offsets[i]));
//    }
//    vec3 col = vec3(0.0);
//    for(int i = 0; i < 9; i++)
//        col += sampleTex[i] * kernel[i];
//    
//    fragmentColour = vec4(col, 1.0);
}