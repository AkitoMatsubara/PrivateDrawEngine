#include "sprite.hlsli"

//#define DRAW_JAPAN // ���̊ە`��

Texture2D diffuse_map : register(t0);   // Diffuse���g�U

SamplerState diffuse_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);      // linear�F���`
SamplerState anisotropic_sampler_state : register(s2); // anisotropic�F�ٕ���

float4 main(PS_IN pin) : SV_TARGET
{
#ifdef DRAW_JAPAN
    {
    const float2 center = float2(1280 / 2, 720 / 2);    // �E�B���h�E�T�C�Y(1280,720)�̒��S�_
    float distance = length(center - pin.positon.xy);   // ���S����xy�̋���
    if (distance > 200) // ���S����200�ȏ㗣��Ă�����
    {
        return 1;       // ���ŕ`�悷��
    }
    else
        return float4(1, 0, 0, 1);  // �Ԃŕ`�悷��
    }
#else
    float4 color = (float4) 0;
    color = diffuse_map.Sample(diffuse_sampler_state, pin.texcoord) * pin.color; // �e�N�X�`�����T���v�����O�A���_�J���[�𔽉f����
    return color;
    //return diffuse_map.Sample(diffuse_sampler_state, pin.texcoord);
#endif
}