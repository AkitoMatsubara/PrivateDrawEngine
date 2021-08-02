#include "skinned_mesh.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 Normal = normalize(pin.world_normal.xyz);
    float3 LighDir = normalize(-light_direction.xyz);
    float3 diffuse = max(0, dot(Normal, LighDir));

    return float4(diffuse, 1) * pin.color;
}