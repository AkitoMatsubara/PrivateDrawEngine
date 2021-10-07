
// 処理の順番としては頂点シェーダ→ピクセルシェーダ

struct VS_IN
{
    float3 position : POSITION; // ローカル位置
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;

};