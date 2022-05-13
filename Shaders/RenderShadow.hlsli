#include "ReflectiveModelFunc.hlsli"
//--------------------------------------------
//	テクスチャ
//--------------------------------------------
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
//cbuffer CBPerFrame2 : register(b2)
//{
//	float4	LightColor;		//ライトの色
//	float4	LightDir;		//ライトの方向
//	float4	AmbientColor;	//環境光
//	float4  EyePos;			//カメラ座標
//};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world;
	float4 material_color;
}

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float4 Position : POSITION;
	float4 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};



//バーテックスシェーダー出力構造体
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//ワールド法線
	float3 wPosition : TEXCOORD2;//ワールド座標
	float2 Tex : TEXCOORD3;
	float3 vShadow : TEXCOORD4;
};