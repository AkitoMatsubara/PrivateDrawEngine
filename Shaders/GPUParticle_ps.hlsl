#include "GPUParticle.hlsli"

Texture2D Tex2D : register(t0); // テクスチャ
SamplerState smpWrap : register(s0); // テクスチャ・サンプラ

float4 main(PS_INPUT Input) : SV_Target
{
    float4 color = Tex2D.Sample(smpWrap, Input.Tex)*Input.Color;
    //color.a *= Input.Life;
    //color = saturate(float4(Input.wPosition, 1.0f));
    return color;
}