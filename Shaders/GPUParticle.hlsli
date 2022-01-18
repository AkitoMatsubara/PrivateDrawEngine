
// 処理の順番としては頂点シェーダ→ピクセルシェーダ

struct VS_IN
{
    float3 position : POSITION; // ローカル位置
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION; // ワールド位置
    float4 world_normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;

};

cbuffer SCENE_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;
}
cbuffer OBJECT_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 world;
}

cbuffer CS_CONSTANT : register(b3)
{
    float3 Size;
    float dummy;
}
