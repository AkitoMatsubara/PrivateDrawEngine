
// 処理の順番としては頂点シェーダ→ピクセルシェーダ

struct VS_IN
{
    float3 NDCPosition : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
};

struct PS_IN
{
    float4 NDCPosition      : SV_POSITION; // ローカル位置
    float3 worldPosition : POSITION;
    float4 color         : COLOR;
};

cbuffer cbScene : register(b0)
{
    float4 viewPosition;
    row_major float4x4 inverseView;
    row_major float4x4 inverseProjection;
}