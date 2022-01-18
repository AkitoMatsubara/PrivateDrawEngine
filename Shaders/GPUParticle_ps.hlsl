#include "GPUParticle.hlsli"

Texture2D diffuse_map : register(t0); // Diffuse＝拡散

SamplerState diffuse_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1); // linear：線形
SamplerState anisotropic_sampler_state : register(s2); // anisotropic：異方性

float4 main(PS_IN pin) : SV_TARGET
{
    //float4 color = (float4) 0;
    //color = diffuse_map.Sample(diffuse_sampler_state, pin.texcoord) * pin.color; // テクスチャをサンプリング、頂点カラーを反映する

    float4 color = diffuse_map.Sample(diffuse_sampler_state, pin.texcoord) * pin.color;

    return color;
}