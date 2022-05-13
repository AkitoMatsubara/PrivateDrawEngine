#include "skinned_mesh.hlsli"

SamplerState sampler_states : register(s0);
Texture2D texture_states : register(t0);


float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_states.Sample(sampler_states, pin.texcoord);
    float3 N = normalize(pin.world_normal);
    float3 E = normalize(camera_position.xyz - pin.world_position.xyz);
    float3 L = normalize(light_direction.xyz);
	// ŠÂ‹«Œõ
	//float3 A = AmbientColor.rgb;
    float3 A = float3(0.5f, 0.5f, 0.5f);

	//ŠgŽU”½ŽË
	//float3 C = LightColor.rgb;
    float3 C = float3(0.5f, 0.5f, 0.5f);
    float3 Kd = float3(1.0f, 1.0f, 1.0f);
    float3 D = Diffuse(N, L, C, Kd);

	//‹¾–Ê”½ŽË
    float3 Ks = float3(1.0f, 1.0f, 1.0f);
    float3 S = PhongSpecular(N, L, C, E, Ks, 20);

    color *= pin.color* float4(A + D + S, 1.0);
    return color;

}