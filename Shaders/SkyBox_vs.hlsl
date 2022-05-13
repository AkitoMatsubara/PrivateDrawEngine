#include "SkyBox.hlsli"

PS_IN main(VS_IN vin) // セマンティクスと一致させて値を受け取る
{
    PS_IN output;
    output.NDCPosition = float4(vin.NDCPosition, 1.0f);
    output.color = vin.color;

    // NDC座標からワールド座標に変換
    float4 p = (float4) 0;

	//	NDC座標からクリップ空間座標への変換
    float4 viewSpacePosition = mul(output.NDCPosition, inverseProjection);
	//	クリップ空間座標からカメラ空間座標への変換
    viewSpacePosition /= viewSpacePosition.w;
	//	カメラ空間座標からワールド空間座標への変換
    p = mul(viewSpacePosition, inverseView);

    output.worldPosition = p.xyz;

    return output;
}