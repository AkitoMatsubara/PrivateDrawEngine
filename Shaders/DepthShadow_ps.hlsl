#include "DepthShadow.hlsli"


//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 main(PSInputShadow input) : SV_Target
{
	// 深度値算出
    float depth = input.Depth.z / input.Depth.w;	// 深度値
    float4 shadow = float4(depth, depth*depth, 0, 1);	// 深度値、深度値の二乗
    return shadow;
}


