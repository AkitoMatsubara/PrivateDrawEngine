#include "DepthShadow.hlsli"

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PSInputShadow main(VSInput input)
{
	PSInputShadow output = (PSInputShadow)0;
	float4 P = input.Position;

	row_major float4x4 matWVP = mul(world, view_projection);
	output.Position = mul(P,matWVP);
	output.Depth = output.Position;
	output.Normal = input.Normal;
	return output;
}

