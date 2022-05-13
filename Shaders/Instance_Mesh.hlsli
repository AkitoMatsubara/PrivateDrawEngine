#include "ReflectiveModelFunc.hlsli"
#include "ShadowFunc.hlsli"

struct VS_IN
{
	float4 position : POSITION; // ローカル頂点位置
	float4 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 world_position : POSITION; // ワールド位置
	float4 world_normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float3 vShadow : TEXCOORD4;
};
