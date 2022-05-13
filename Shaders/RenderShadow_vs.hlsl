#include "RenderShadow.hlsli"
#include "ShadowFunc.hlsli"

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;
    float4 P = input.Position; // float3のPositionをfloat4に変換
	row_major float4x4 matWVP = mul(world, view_projection);	// WVP算出

    output.Position = mul(P, matWVP); // カメラからの位置を算出

	float3 wPos = mul(P,world).xyz;	// ワールド座標算出
	output.wPosition = wPos;

	float3 wN = mul(world, input.Normal).xyz;	// ワールド法線
	wN = normalize(wN);	// 正規化
	output.wNormal = wN;

	output.Tex = input.Tex;
	output.Color = material_color;

	// UV座標を算出する
    output.vShadow = GetShadowTex(LightViewProjection, wPos);

	return output;
}

