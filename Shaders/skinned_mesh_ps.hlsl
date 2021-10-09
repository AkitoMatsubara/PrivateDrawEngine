#include "skinned_mesh.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_states[3] : register(t0);


float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_states[0].Sample(sampler_states[POINT], pin.texcoord);
    float3 Normal = normalize(pin.world_normal.xyz);
    float3 LighDir = normalize(-light_direction.xyz);
    float3 diffuse = color.rgb * max(0, dot(Normal, LighDir));

    return float4(diffuse, color.a) * pin.color;

}