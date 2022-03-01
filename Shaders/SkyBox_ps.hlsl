#include "SkyBox.hlsli"


Texture2D Image : register(t0);

SamplerState diffuse_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1); // linear�F���`
SamplerState anisotropic_sampler_state : register(s2); // anisotropic�F�ٕ���

float4 main(PS_IN pin) : SV_TARGET
{
	//	�J�������烏�[���h���W�ւ̃x�N�g�����Z�o���A��������ʏ�̍��W�Ƃ��Ĉ���
    // �����x�N�g��
    float3 E = normalize(pin.worldPosition.xyz - viewPosition.xyz);

	//	�����x�N�g����UV���W�֕ϊ�����
    float2 texcoord;
	{
        static const float PI = 3.141592654f;
        texcoord = float2(1.0f - atan2(E.z, E.x) / 2, -atan2(E.y, length(E.xz)));
        texcoord = texcoord / PI + 0.5f;
    }
    return Image.Sample(diffuse_sampler_state, texcoord) * pin.color;
}