#include "SkyBox.hlsli"

PS_IN main(VS_IN vin) // セマンティクスと一致させて値を受け取る
{
    PS_IN vout;
    vout.NDCPosition = float4(vin.NDCPosition, 1.0f);
    vout.color = vin.color;

    // NDC座標からワールド座標に変換
    float4 p;

	//	NDC座標からクリップ空間座標への変換
    float4 viewSpacePosition = mul(vout.NDCPosition, inverseProjection);
	//	クリップ空間座標からカメラ空間座標への変換
    viewSpacePosition /= viewSpacePosition.w;
	//	カメラ空間座標からワールド空間座標への変換
    p = mul(viewSpacePosition, inverseView);

    vout.worldPosition = p.xyz;

    return vout;
}