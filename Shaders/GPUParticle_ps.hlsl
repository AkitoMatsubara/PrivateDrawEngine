#include "GPUParticle.hlsli"

Texture2D diffuse_map : register(t0); // Diffuse���g�U

SamplerState diffuse_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1); // linear�F���`
SamplerState anisotropic_sampler_state : register(s2); // anisotropic�F�ٕ���

float4 main(PS_IN pin) : SV_TARGET
{
    //float4 color = (float4) 0;
    //color = diffuse_map.Sample(diffuse_sampler_state, pin.texcoord) * pin.color; // �e�N�X�`�����T���v�����O�A���_�J���[�𔽉f����

    float4 color = diffuse_map.Sample(diffuse_sampler_state, pin.texcoord) * pin.color;

    return color;
}