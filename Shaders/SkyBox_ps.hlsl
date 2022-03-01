#include "SkyBox.hlsli"


Texture2D Image : register(t0);

SamplerState diffuse_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1); // linear：線形
SamplerState anisotropic_sampler_state : register(s2); // anisotropic：異方性

float4 main(PS_IN pin) : SV_TARGET
{
	//	カメラからワールド座標へのベクトルを算出し、それを球面上の座標として扱う
    // 視線ベクトル
    float3 E = normalize(pin.worldPosition.xyz - viewPosition.xyz);

	//	視線ベクトルをUV座標へ変換する
    float2 texcoord;
	{
        static const float PI = 3.141592654f;
        texcoord = float2(1.0f - atan2(E.z, E.x) / 2, -atan2(E.y, length(E.xz)));
        texcoord = texcoord / PI + 0.5f;
    }
    return Image.Sample(diffuse_sampler_state, texcoord) * pin.color;
}