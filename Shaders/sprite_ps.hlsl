#include "sprite.hlsli"

//#define JAPAN // 日の丸描画

float4 main(VS_OUT pin) : SV_TARGET
{
#ifdef JAPAN
    const float2 center = float2(1280 / 2, 720 / 2);    // ウィンドウサイズ(1280,720)の中心点
    float distance = length(center - pin.positon.xy);   // 中心からxyの距離
    if (distance > 200) // 中心から200以上離れていたら
    {
        return 1;       // 白で描画する
    }
    else
        return float4(1, 0, 0, 1);  // 赤で描画する
#else
    return pin.color; // 頂点情報の色を渡す
#endif
}