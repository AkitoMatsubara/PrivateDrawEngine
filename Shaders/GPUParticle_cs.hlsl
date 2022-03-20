//// http://maverickproj.web.fc2.com/d3d11_17.html

//// SV_GroupThreadID    : x      , y       , z      //スレッドID　ここで指定
//// SV_GroupID          : x'     , y'      , z'     //グループID　ディスパッチ側で指定
//// SV_DispatchThreadID : x'* X + x , y'* Y+y , z'* Y+ z
//// SV_GroupIndex       : z * X * Y + y * X + x

#include "GPUParticle.hlsli"



static float3 Acceleration = float3(0.0, -4.8, 0.0); // 加速度(m/s^2)

// 入力バッファ(構造化バッファ。読み込み専用)
StructuredBuffer<VS_INPUT_GPU> Input : register(t0);
// 出力バッファ(構造化バッファ。読み書き可能)
RWStructuredBuffer<VS_INPUT_GPU> Result : register(u0);


//スレッドの数
#define TH_X 500
#define TH_Y 1
#define TH_Z 1


// シェーダ関数
[numthreads(TH_X, TH_Y, TH_Z)]
void main(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID)
{
    int THG_X = ParticleNo; // dispatch数

    uint x = GTid.x;
    uint y = Gid.x;

    uint node = TH_X * y + x;

    float3 myLocation = Input[node].Position; //自分の位置
//	const float length = 0.2f;//近くの点の判断用
    int count = 0;

    float  nearest_Length  = 9999999.0f;
    float3 nearest_Pos     = float3(0, 0, 0);
    float3 Total_Pos       = float3(0, 0, 0);
    float3 Total_Direction = float3(0, 0, 0);
    float3 answer;

    float MAX_ACCEL = 0.001f;
    float MAX_SPEED = 0.05f;

//	//int count = 10000;// = TH_X * TH_X;
//    for (int i = 0; i < THG_X * TH_X; i++)
//    {
//        float3 near_Location = Input[i].Position;

//        float ans = distance(near_Location, myLocation);

////		if (ans < length) //近い点を抽出する。
//		{
//            if (ans != 0)
//            {
//                if (nearest_Length > ans)//一番近い点を記憶
//                {
//                    nearest_Length = ans;
//                    nearest_Pos = Input[i].Position;

//                }
//            }
//            Total_Pos += Input[i].Position;
//            Total_Direction += Input[i].Velocity;

//            count++;
//        }
//    }
//    if (count != 0)
//    {
//        float3 centerPos = Total_Pos / (float) count;
//        answer = normalize(centerPos - myLocation) * 0.04f;
//        answer += normalize(Total_Direction) * 0.02f;
//        answer += normalize(nearest_Pos - myLocation) * -0.12f;

//    }
//    else
//    {
//        answer = float3(0, 0, 0);
//    }

//	//最大スピード = 3.0f;
//	//最大加速度 = 0.1f;
//    float3 F = Input[node].Force;
//    F += answer;
//    if (length(F) > MAX_ACCEL)
//    {
//        F = normalize(F);
//        F *= MAX_ACCEL;
//    }
//    float3 V = Input[node].Velocity;
//    V += F;
//    if (length(V) > MAX_SPEED)
//    {
//        V = normalize(V);
//        V *= MAX_SPEED;
//    }
//    float3 P = Input[node].Position;
//    P += V;

//    float val = 0.15f;

//    if (P.x < -8) F.x = normalize(-myLocation).x * val;
//    if (P.y < 2)  F.y = normalize(-myLocation).y * val;
//    if (P.z < -8) F.z = normalize(-myLocation).z * val;
//    if (P.x > 8)  F.x = normalize(-myLocation).x * val;
//    if (P.y > 3)  F.y = normalize(-myLocation).y * val;
//    if (P.z > 8)  F.z = normalize(-myLocation).z * val;
//    Result[node].Force = F;
//    Result[node].Velocity = V;
//    Result[node].Position = P;

	{
        // 生存時のみ情報を更新する
    	if (Input[node].Active)
    	{
    		// 見やすいように一度変数に
    		float3 pos = Input[node].Position;
    		float3 vel = Input[node].Velocity;
    		float3 force = Input[node].Force;
            float life = Input[node].Life;
    		pos += vel; // 位置更新
    		vel += force; // 速度更新
    		// CPUに返す
    		Result[node].Velocity = vel;
    		Result[node].Position = pos;
            Result[node].Color = Input[node].Color;

            // ライフ計算、死んだら非生存に
            Result[node].Life = life - 1.0f;
    		if (Result[node].Life <= 0.0f)
    		{
    			Result[node].Active = false;
                Result[node].Color = float4(1, 0, 0, 0.6);
            }
            else
            {
                Result[node].Active = true;
            }
        }
    }
}
