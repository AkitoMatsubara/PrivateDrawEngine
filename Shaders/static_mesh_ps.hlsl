#include "static_mesh.hlsli"

Texture2D color_map : register(t0);
Texture2D normal_map : register(t1);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

// �ق�����܂��҂񂮂�ps pdf.3�̂Ȃ��悤�Ȃ̂ł����܂ŗ����炱��R�����g�A�E�g���悤��
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord); // �e�N�X�`���̎擾
    float alpha = color.a;
    float3 Normal = normalize(pin.world_normal.xyz); // ���[���h���W�n�̖@���𐳋K��

    float3 T = float3(1.0001, 0, 0);
    float3 B = normalize(cross(Normal, T));
    T = normalize(cross(B, Normal));
    float4 normal = normal_map.Sample(linear_sampler_state, pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    normal.w = 0;
    Normal = normalize((normal.x * T) + (normal.y * B) + (normal.z * Normal));

    // �g�U����
    float3 Light = normalize(-light_direction.xyz); // ���C�g�̔��Ε����Ő��K��
    float3 diffuse = color.rgb * max(0, dot(Normal, Light)); //Diffuse = �g�U����
    // ���ʔ���
    float3 V = normalize(camera_position.xyz - pin.world_position.xyz); // ���[���h���W�n�ʒu����J�����ւ̃x�N�g��
    float3 specular = pow(max(0, dot(Normal, normalize(V + Light))), 128); // Speqular = ����  �ݏ悷��l���傫���قǃ����n�����ł���

    return float4(diffuse + specular, alpha) * pin.color;
}