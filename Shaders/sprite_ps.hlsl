#include "sprite.hlsli"

//#define DRAW_JAPAN // 日の丸描画

Texture2D diffuse_map : register(t0);   // Diffuse＝拡散

SamplerState diffuse_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);      // linear：線形
SamplerState anisotropic_sampler_state : register(s2); // anisotropic：異方性

float4 main(PS_IN pin) : SV_TARGET
{
#ifdef DRAW_JAPAN
    {
    const float2 center = float2(1280 / 2, 720 / 2);    // ウィンドウサイズ(1280,720)の中心点
    float distance = length(center - pin.positon.xy);   // 中心からxyの距離
    if (distance > 200) // 中心から200以上離れていたら
    {
        return 1;       // 白で描画する
    }
    else
        return float4(1, 0, 0, 1);  // 赤で描画する
    }
#else
    float4 color = (float4) 0;
    color = diffuse_map.Sample(diffuse_sampler_state, pin.texcoord) * pin.color; // テクスチャをサンプリング、頂点カラーを反映する
    return color;
    //return diffuse_map.Sample(diffuse_sampler_state, pin.texcoord);
#endif
}