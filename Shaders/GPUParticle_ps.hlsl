#include "GPUParticle.hlsli"

//--------------------------------------------
//	フォグ
//--------------------------------------------
//color		:現在の色
//viewPos	:視点
//Pos		:ピクセルの位置
//fogColor	:フォグの色
//Near		:フォグの近距離
//Far		:フォグの遠距離
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far)
{
    float Len = length(viewPos - Pos);
    float fogalpha = saturate((Len - Near) / (Far - Near));
    return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}

// ピクセル・シェーダの関数
float4 main(PS_INPUT Input) : SV_Target
{
    float4 color = Tex2D.Sample(smpWrap, Input.Tex);
    //color.xyz = Fog(color.xyz, EyePos.xyz, Input.wPosition, FogColor.xyz, FogNear, FogFar);

    return color;
//				float4 vec1 = normalize(Input.Pos);
//				return float4(vec1.x, vec1.y, vec1.z, 1);
}