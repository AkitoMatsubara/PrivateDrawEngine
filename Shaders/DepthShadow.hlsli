//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world;
	float4 material_color;
}

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 view_projection;
	float4 light_direction;
	float4 camera_position;
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
struct PSInputShadow
{
	float4 Position : SV_POSITION;
	float4 Depth : TEXCOORD1;	//深度値
	float3 Normal : NORMAL;
};


