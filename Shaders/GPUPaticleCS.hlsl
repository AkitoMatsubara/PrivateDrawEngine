#include "GPUParticle.hlsli"

// 入力用の構造体定義
struct BufInType
{
    float3 Pos;
    float dummy;
};

// 出力用の構造体定義
struct BufOutType
{
    float3 Pos;
    float dummy;
    float3 Size;
    float dummy2;
};

// 読み取り専用の構造化バッファー
// アンオーダードアクセスバッファーとして作成されたバッファを使用する
StructuredBuffer<BufInType> BufferIn : register(t1);

// 読み書き可能な構造化バッファー
// アンオーダードアクセスバッファーとして作成されたバッファを使用する
RWStructuredBuffer<BufOutType> BufferOut : register(u1);

[numthreads(1, 1, 1)] // 列、行、枚数
void main(uint3 DTid : SV_DispatchThreadID) // SV_DispatchThreadID：SV_GroupID * Numthreads と groupthreadid の合計
{
    BufferOut[DTid.x].Size = Size;

}

// http://maverickproj.web.fc2.com/d3d11_17.html

// SV_GroupThreadID    : x      , y       , z
// SV_GroupID          : x'     , y'      , z'
// SV_DispatchThreadID : x'* X + x , y'* Y+y , z'* Y+ z
// SV_GroupIndex       : z * X * Y + y * X + x