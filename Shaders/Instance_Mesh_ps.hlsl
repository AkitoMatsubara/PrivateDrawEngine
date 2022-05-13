#include "Instance_Mesh.hlsli"

SamplerState DecalSampler : register(s0);

float4 main(VS_OUT input) : SV_Target
{
	static const float Bias = 0.00005;

	float4 color = MappingTexture.Sample(DecalSampler, input.texcoord);
	float3 N = normalize(input.world_normal);
	float3 E = normalize(camera_position.xyz - input.world_position.xyz);
	float3 L = normalize(light_direction.xyz);
	// ����
	float3 A = float3(0.5f, 0.5f, 0.5f);

	//�g�U����
	//float3 C = LightColor.rgb;
	float3 C = float3(0.5f, 0.5f, 0.5f);
	float3 Kd = float3(1.0f, 1.0f, 1.0f);
	float3 D = Diffuse(N, L, C, Kd);

	//���ʔ���
	float3 Ks = float3(1.0f, 1.0f, 1.0f);
	float3 S = PhongSpecular(N, L, C, E, Ks, 20);

	color *= input.color * float4(A + D + S, 1.0);

	// �V���h�E�}�b�v�K�p
	float shadowDepth = GetShadow(ShadowTexture, DecalSampler, input.vShadow, ShadowColor.rgb, Bias); // �e�̔Z��
	color.rgb *= shadowDepth / 2;
    if (color.a <= 0.0f) discard;
	return color;
}


