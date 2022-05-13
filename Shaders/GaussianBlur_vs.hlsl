#include "GaussianBlur.hlsli"

PS_IN main(VS_IN vin)
{
    PS_IN vout;
    vout.position = float4(vin.position, 1); // そのままピクセルシェーダに流す
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
    return vout;
}