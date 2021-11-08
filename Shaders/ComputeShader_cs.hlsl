
cbuffer CS_CONSTANT : register(b2)
{
    float Theta;
    float3 dummy;
}
// 入力用の構造体定義
struct BufInType
{
    int i;
    float f;
};

// 出力用の構造体定義
struct BufOutType
{
    float i;
};

// 読み取り専用の構造化バッファー
// アンオーダードアクセスバッファーとして作成されたバッファを使用する
StructuredBuffer<BufInType> BufferIn : register(t0);

// 読み書き可能な構造化バッファー
// アンオーダードアクセスバッファーとして作成されたバッファを使用する
RWStructuredBuffer<BufOutType> BufferOut : register(u0);

[numthreads(1, 1, 1)]   // 列、行、枚数
void main(uint3 DTid : SV_DispatchThreadID) // SV_DispatchThreadID：SV_GroupID * Numthreads と groupthreadid の合計
{
    //BufferOut[DTid.x].i = 10;
    //BufferOut[DTid.x].i = BufferIn[DTid.x].i + (int) BufferIn[DTid.x].f;
    BufferOut[DTid.x].i = abs(sin(Theta) / (DTid.x + 1));
}

// http://maverickproj.web.fc2.com/d3d11_17.html

// SV_GroupThreadID    : x      , y       , z
// SV_GroupID          : x'     , y'      , z'
// SV_DispatchThreadID : x'* X + x , y'* Y+y , z'* Y+ z
// SV_GroupIndex       : z * X * Y + y * X + x