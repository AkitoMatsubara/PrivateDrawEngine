#include "GaussianBlur.hlsli"

Texture2D DiffuseTexture : register(s0);
SamplerState DecalSampler: register(s0);

float4 main(PS_IN input):SV_Target
{
    float4 color = (float4) 0;
    color.a = 1.0f;
    for (int i = 0; i < KarnelSize * KarnelSize; i++)
    {
        float2 offset = Texcel * Weight[i].xy;
        float weight = Weight[i].z;
        color.rgb += DiffuseTexture.Sample(DecalSampler, input.texcoord + offset).rgb * weight;
    }
    return color;
}
