static const uint poissonDiskSamples = 12;
static const float2 poissonDisk[poissonDiskSamples] = {
    float2(-0.48787218535567783f, -0.5767200429457077f ),
    float2( 0.5123598560741673f,   0.8346124641528023f ),
    float2( 0.8000167922626604f,  -0.5946706164572381f ),
    float2(-0.8051259385127932f,   0.5530995226035568f ),
    float2( 0.16830167451832398f,  0.012105119424310406f ),
    float2( 0.9639023832151082f,   0.08597819366621468f ),
    float2( 0.1132753341919733f,  -0.9741415264133771f ),
    float2(-0.08757834586721547f,  0.6607989331895558f ),
    float2(-0.962934267307499f,   -0.08687841233111343f ),
    float2(-0.4073656890509206f,   0.16453403622594887f ),
    float2( 0.5805369551894531f,   0.3366656651890103f ),
    float2( 0.13623426653302975f, -0.5176471522658811f ),
};

float GenerateShadow_PCF(Texture2DArray tex, SamplerState state, float3 projectionCoords, float lightIndex, float shadowOffsetBias){
    float currentDepth = projectionCoords.z;
	
	float width, height, elements;
	tex.GetDimensions(width, height, elements);
	const float2 texelSize = 1.0f / float2(width, height);
	
	float shadow = 0.0f;
	for(uint i = 0; i < poissonDiskSamples; ++i){
		const float2 sampleLocation = projectionCoords.xy + poissonDisk[i] * texelSize;
		float closestDepth = tex.Sample(state, float3(sampleLocation, lightIndex)).r;
		shadow += currentDepth - shadowOffsetBias > closestDepth ? 1.0f : 0.0f;
	}	
	shadow /= poissonDiskSamples;
    
    return shadow;
}