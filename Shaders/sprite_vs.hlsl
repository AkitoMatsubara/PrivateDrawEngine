#include "sprite.hlsli"

PS_IN main(VS_IN vin) // セマンティクスと一致させて値を受け取る
{
    PS_IN vout;
    vout.position = float4(vin.position, 1); // 現状は処理を行わないため、そのままピクセルシェーダに流す
    vout.color    = vin.color;
    vout.texcoord = vin.texcoord;
    return vout;
}