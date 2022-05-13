#include "Instance_Mesh.hlsli"

struct InBuffer {	// インスタンス描画用パラメータ
    row_major float4x4 world; // ワールド行列 オブジェクトによって位置や回転が違うので複数必要
    float4 color;	// 今回は色もそれぞれ違うので受け取る
};

// 入力バッファ(構造化バッファ。読み込み専用)
StructuredBuffer<InBuffer> InputBuffer : register(t0);

VS_OUT main(VS_IN vin)
{
	VS_OUT vout = (VS_OUT)0;
    float4 pos = vin.position;
	float4x4 World = InputBuffer[vin.instanceID].world;
    float4 color = InputBuffer[vin.instanceID].color;

	vout.position = mul(pos, mul(World, view_projection));
	vout.world_position = mul(pos, World); // ワールド変換
	vin.normal.w = 0;
	vout.world_normal = normalize(mul(vin.normal, World)); // ワールド変換したやつを正規化
	vout.texcoord = vin.texcoord;
	vout.color = color;

	float3 wPos = mul(pos, World).xyz; // ワールド座標算出

	// UV座標を算出する
	vout.vShadow = GetShadowTex(LightViewProjection, wPos);
	return vout;
}