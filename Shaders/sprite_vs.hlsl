
#include "sprite.hlsli"

VS_OUT main( float4 position : POSITION ,float4 color:COLOR) // セマンティクスと一致させて値を受け取る
{
    VS_OUT vout;
    vout.positon = position;    // 現状は処理を行わないため、そのままピクセルシェーダに流す
    vout.color = color;
    return vout;
}