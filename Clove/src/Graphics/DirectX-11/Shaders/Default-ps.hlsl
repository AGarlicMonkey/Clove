Texture2D diffuseTexture : register(t1);
SamplerState diffuseSampler : register(s1);

Texture2D specularTexture : register(t2);
SamplerState specularSampler : register(s2);

cbuffer PointLightBuffer : register(b1){
    float3 position;

    float3 ambient;
    float3 diffuse;
    float3 specular;

    float constant;
    float linearV;
    float quadratic;
};

cbuffer ViewBuffer : register(b2){
    float3 viewPos;
}

cbuffer MaterialBuffer : register(b4){
    float shininess;
}

float4 main(float2 texCoord : TexCoord, float3 vertPos : VertPos, float3 vertNormal : VertNormal) : SV_Target{
    float3 normal       = normalize(vertNormal);
    float3 viewDir      = normalize(viewPos - vertPos);
    
    float3 lightDir     = normalize(position - vertPos);

    //Ambient
    float3 lambient     = ambient * (float3)diffuseTexture.Sample(diffuseSampler, texCoord);

    //Diffuse
    float3 diff         = max(dot(normal, lightDir), 0.0f);
    float3 ldiffuse     = diffuse * diff * (float3)diffuseTexture.Sample(diffuseSampler, texCoord);

    //Specular
    float3 reflectDir   = reflect(-lightDir, normal);
    float spec          = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    float3 lspecular    = specular * spec * (float3)specularTexture.Sample(specularSampler, texCoord);
    
    //Attenuation
    float distance      = length(position - vertPos);
    float attenuation   = 1.0f / (constant + (linearV * distance) + (quadratic * (distance * distance)));

    lambient    *= attenuation;
    ldiffuse    *= attenuation;
    lspecular   *= attenuation;

    return float4((lambient + ldiffuse + lspecular), 1.0f);
}