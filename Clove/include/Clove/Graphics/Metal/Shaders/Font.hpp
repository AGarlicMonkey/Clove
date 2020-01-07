//cmake generated header file

static const std::string shader_Font = R"(
#include <metal_stdlib>
using namespace metal;

struct viewBuffer{
	float4x4 modelProjection;
};

struct VSIn{
	float2 Position2D [[ attribute(0) ]];
	float2 TexCoord [[ attribute(1) ]];
};

struct VSOut{
	float4 position [[ position ]];
	float2 texCoord;
};

vertex VSOut vertexShader(const VSIn vertexIn [[ stage_in ]], constant viewBuffer& viewBuffer [[ buffer(6) ]]){
	VSOut out;
	
	out.position = viewBuffer.modelProjection * float4(vertexIn.Position2D.x, vertexIn.Position2D.y, 0.0f, 1.0f);
	out.texCoord = vertexIn.TexCoord;
	
	return out;
}

fragment half4 pixelShader(const VSOut vertexIn [[ stage_in ]], texture2d<float> albedoTexture [[ texture(1) ]]){
	constexpr sampler defaultSampler;
	return half4(1.0f, 1.0f, 1.0f, albedoTexture.sample(defaultSampler, vertexIn.texCoord).r);
}

)";