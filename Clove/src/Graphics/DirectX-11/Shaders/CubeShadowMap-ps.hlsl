cbuffer lighthPosBuffer : register (b7){
	float3 lightPosition;
	float farplane;
}

float main(float4 vertPosition : VertPos) : SV_Depth{
	const float lightDistance = length(vertPosition.xyz - lightPosition) / farplane;
	return lightDistance;
}