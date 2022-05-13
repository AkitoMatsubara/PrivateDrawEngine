//// http://maverickproj.web.fc2.com/d3d11_17.html

//// SV_GroupThreadID    : x      , y       , z      //スレッドID　処理中のnumthread番号
//// SV_GroupID          : x'     , y'      , z'     //グループID　処理中のDispatch番号
//// SV_DispatchThreadID : x' * X + x , y' * Y+y , z' * Y+ z   // Dispatch*numthread+GTid
//// SV_GroupIndex       : z * X * Y + y * X + x

#include "GPUParticle.hlsli"



static float3 Acceleration = float3(0.0, -4.8, 0.0); // 加速度(m/s^2)

// 入力バッファ(構造化バッファ。読み込み専用)
StructuredBuffer<VS_INPUT_GPU> InBuffer : register(t0);
// 出力バッファ(構造化バッファ。読み書き可能)
RWStructuredBuffer<VS_INPUT_GPU> Result : register(u0);


// floatの値を受け取って乱数生成 -range~rangeの間の数を生成
float rand(float theta, float range)
{
	return (frac(sin(theta) * 100000.0) * 2.0 - 1.0) * range;
}
// float2の値を受け取って乱数生成  -range~rangeの間の数を生成
float rand(float2 theta, float range)
{
	return (frac(sin(dot(theta.xy, float2(12.9898, 78.233))) * 10000.0) * 2.0 - 1.0) * range;
}
// floatの2値を受け取って乱数生成
float3 rand3(float2 seed, float range)
{
	return 2.0 * (float3(rand(seed * 1, range), rand(seed * 2, range), rand(seed * 3, range)) - 0.5);
}

// パーティクル再設定
VS_INPUT_GPU Reset(uint3 dTid, float seed1)
{
    uint node = dTid.x;

    float2 seed = float2(dTid.x + 1, dTid.y + 2) * InBuffer[node].Position.xz; // 乱数用seed
    float3 pos = rand3(seed, 10.0); // 乱数から出した位置
    pos.y = rand(seed * pos.xy, 5.0); // yだけ低めに調整
    
    float3 vel = rand3(seed, 50.0) * 0.0005;
    vel.y = rand(seed, 5.0) * 0.0005; // 同じく加速度もyだけ低めに調整
    
    float3 force = rand3(vel.xz, 50.0) * 0.0000001;

    VS_INPUT_GPU p = InBuffer[node];
    p.Position.xyz = pos;
    p.Velocity = vel;
    p.Force = force;
    //p.Color = float4(InBuffer[node].Color.rgb,0.0f);
    p.Color = float4(rand3(pos.rg,1.0), 0.0f);  // 色ランダム
	p.Life = uint(rand(pos.z, 5.0f)); // unsigned
    p.Active = true;
    return p;
}

//スレッドの数
#define TH_X 500
#define TH_Y 1
#define TH_Z 1
// シェーダ関数
[numthreads(TH_X, TH_Y, TH_Z)]
void main(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint3 DTid : SV_DispatchThreadID)
{
	int THG_X = ParticleNo; // dispatch数

	uint x = GTid.x;
	uint y = Gid.x;

	//uint node = TH_X * Gid.x + GTid.x;
	uint node = DTid.x;

		// 生存時のみ情報を更新する
    if (InBuffer[node].Active)
    {
		// 見やすいように(計算するために)一度変数に
        float4 pos = InBuffer[node].Position;
        float3 vel = InBuffer[node].Velocity;
        float3 force = InBuffer[node].Force;
        float life = InBuffer[node].Life;
		float alpha = sin((life / pos.w) * 3.15f) * 1.5f; // 現在ライフを最大ライフで除算、PIをかけると半分の大きさのサイン波ができる　そこに掛け算すると値の増減が早まる
		life -= 0.01f;
        pos.xyz += vel; // 位置更新
        vel += force; // 速度更新
		
        // CPUに返す
		Result[node].Position = pos;
        Result[node].Velocity = vel;
		Result[node].Force = force;
        Result[node].Color = InBuffer[node].Color;
		Result[node].Color.w = alpha;
        Result[node].Life = life;
		  // ライフ計算、死んだら非生存に
		Result[node].Active = (Result[node].Life <= 0.0f) ? false : true;
    }
    // 非生存なら再設定を行って生存させる
    else
    {
		Result[node] = Reset(DTid, InBuffer[node].Position.x);
	}
}
