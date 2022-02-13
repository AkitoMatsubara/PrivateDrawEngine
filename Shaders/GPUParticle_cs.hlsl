//#include "GPUParticle.hlsli"

//// 入力用の構造体定義
//struct BufInType
//{
//    float3 Pos;
//    float dummy;
//};

//// 出力用の構造体定義
//struct BufOutType
//{
//    float3 Pos;
//    float dummy;
//    float3 Size;
//    float dummy2;
//};

//// 読み取り専用の構造化バッファー
//// アンオーダードアクセスバッファーとして作成されたバッファを使用する
//StructuredBuffer<BufInType> BufferIn : register(t1);

//// 読み書き可能な構造化バッファー
//// アンオーダードアクセスバッファーとして作成されたバッファを使用する
//RWStructuredBuffer<BufOutType> BufferOut : register(u1);

//[numthreads(1, 1, 1)] // 列、行、枚数
//void main(uint3 DTid : SV_DispatchThreadID) // SV_DispatchThreadID：SV_GroupID * Numthreads と groupthreadid の合計
//{
//    BufferOut[DTid.x].Size = Size;

//}

//// http://maverickproj.web.fc2.com/d3d11_17.html

//// SV_GroupThreadID    : x      , y       , z
//// SV_GroupID          : x'     , y'      , z'
//// SV_DispatchThreadID : x'* X + x , y'* Y+y , z'* Y+ z
//// SV_GroupIndex       : z * X * Y + y * X + x
///
///
#include "GPUParticle.hlsli"


//CPUへの書き込み
struct ReturnBuffer
{
    int toCpuNo; //一番多い場所
    float3 toCpuPosition; // 座標値
};


static float3 Acceleration = float3(0.0, -4.8, 0.0); // 加速度(m/s^2)

// 入力バッファ(構造化バッファ。読み込み専用)
StructuredBuffer<GS_INPUT_GPU2> Input : register(t0);
// 出力バッファ(構造化バッファ。読み書き可能)
RWStructuredBuffer<GS_INPUT_GPU2> Result : register(u0);
RWStructuredBuffer<ReturnBuffer> Result2 : register(u1);


//スレッドグループの数（CPU側でディスパッチした数）
//#define THG_X 10

//スレッドの数
#define TH_X 200
#define TH_Y 1
#define TH_Z 1



// シェーダ関数
[numthreads(TH_X, TH_Y, TH_Z)] // スレッド グループのスレッド数
void main(uint3 Gid : SV_GroupID, //グループID　ディスパッチ側で指定
//	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID //スレッドID　ここで指定
//	uint GI : SV_GroupIndex
)
{
	//☆
    int THG_X = ParticleNo;

    uint x = GTid.x;
    uint y = Gid.x;
//	uint  CULMS_NO = TH_X;

    uint node = TH_X * y + x;

    float3 myLocation = Input[node].Position; //自分の位置
//	const float length = 0.2f;//近くの点の判断用
    int count = 0;

    float nearest_Length = 9999999.0f;
    float3 nearest_Pos = float3(0, 0, 0);
    float3 Total_Pos = float3(0, 0, 0);
    float3 Total_Direction = float3(0, 0, 0);
    float3 answer;

    float MAX_ACCEL = 0.001f;
    float MAX_SPEED = 0.05f;

	//int count = 10000;// = TH_X * TH_X;
    for (int i = 0; i < THG_X * TH_X; i++)
    {
        float3 near_Location = Input[i].Position;

		//float2  v = near_Location - myLocation;
        float ans = distance(near_Location, myLocation);

//		if (ans < length) //近い点を抽出する。
		{
            if (ans != 0)
            {
                if (nearest_Length > ans)//一番近い点を記憶
                {
                    nearest_Length = ans;
                    nearest_Pos = Input[i].Position;

                }
            }
            Total_Pos += Input[i].Position;
            Total_Direction += Input[i].Velocity;

            count++;
        }
    }
    if (count != 0)
    {
        float3 centerPos = Total_Pos / (float) count;
        answer = normalize(centerPos - myLocation) * 0.04f;
        answer += normalize(Total_Direction) * 0.02f;
        answer += normalize(nearest_Pos - myLocation) * -0.12f;

    }
    else
    {
        answer = float3(0, 0, 0);
    }

	//最大スピード = 3.0f;
	//最大加速度 = 0.1f;
    float3 F = Input[node].Force;
    F += answer;
    if (length(F) > MAX_ACCEL)
    {
        F = normalize(F);
        F *= MAX_ACCEL;
    }
    float3 V = Input[node].Velocity;
    V += F;
    if (length(V) > MAX_SPEED)
    {
        V = normalize(V);
        V *= MAX_SPEED;
    }
    float3 P = Input[node].Position;
    P += V;

    float val = 0.15f;

    if (P.x < -8)
        F.x = normalize(-myLocation).x * val;
    if (P.y < 2)
        F.y = normalize(-myLocation).y * val;
    if (P.z < -8)
        F.z = normalize(-myLocation).z * val;
    if (P.x > 8)
        F.x = normalize(-myLocation).x * val;
    if (P.y > 3)
        F.y = normalize(-myLocation).y * val;
    if (P.z > 8)
        F.z = normalize(-myLocation).z * val;
    Result[node].Force = F;
    Result[node].Velocity = V;
    Result[node].Position = P;

    if (node == 1)
    {
        Result2[0].toCpuNo = 123; //一番多い場所
        Result2[0].toCpuPosition = Total_Pos / (float) count;
		//	Result2[0].toCpuPosition.x = 0;      // 座標値
		//	Result2[0].toCpuPosition.y = 0;      // 座標値
		//	Result2[0].toCpuPosition.z = 0;      // 座標値
    }
}
