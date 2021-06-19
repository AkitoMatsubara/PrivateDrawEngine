
// 処理の順番としては頂点シェーダ→ピクセルシェーダ

struct VS_OUT
{
    float4 positon : SV_POSITION;
    float4 color   : COLOR;
};