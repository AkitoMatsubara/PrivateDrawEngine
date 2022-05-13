#include "GPUParticle.hlsli"

Texture2D Tex2D : register(t0);
SamplerState smpWrap : register(s0);

float4 main(PS_INPUT Input) : SV_Target
{
	float4 color = Tex2D.Sample(smpWrap, Input.Tex) * Input.Color;
	if (color.a <= 0.0)discard;
		return color;
}