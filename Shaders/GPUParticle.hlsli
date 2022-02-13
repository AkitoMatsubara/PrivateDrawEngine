
//// 処理の順番としては頂点シェーダ→ピクセルシェーダ

//struct VS_IN
//{
//    float3 position : POSITION; // ローカル位置
//    float3 normal : NORMAL;
//    float2 texcoord : TEXCOORD;
//    float4 color : COLOR;
//};

//struct PS_IN
//{
//    float4 position : SV_POSITION;
//    float4 color : COLOR;
//    float3 world_position : POSITION; // ワールド位置
//    float2 texcoord : TEXCOORD;
//};

//// VSシェーダの入力データ定義
//struct GS_INPUT_GPU2
//{
//    float3 position : POSITION; // 頂点座標(ワールド座標系)
//    float3 velocity : VELOCITY; // 現在の速度ベクトル(ワールド座標系)
//    float3 force : FORCE; // 現在の加速度
//};

//// GSシェーダの入力データ定義
//struct GS_INPUT
//{
//    float4 Pos : SV_POSITION; // 頂点座標(射影座標系)
//    float2 Tex : TEXTURE; // テクスチャ座標

//};
//cbuffer global
//{
//    matrix View; // ビュー変換行列
//    matrix Projection; // 透視変換行列
//    float2 ParticleSize; // パーティクルの大きさ
//    int ParticleNo; //大きさ
//    float dummy; // ダミー

//    float4 EyePos; //カメラ座標
//    float4 FogColor; //霧の色
//    float FogNear; //霧の始まる位置
//    float FogFar; //霧の終わる位置
//    float dummy1;
//    float dummy2;

//}

//cbuffer SCENE_CONSTANT_BUFFER : register(b0)
//{
//    row_major float4x4 view_projection;
//    matrix View;
//    matrix Projection;

//    float4 light_direction;
//    float4 camera_position;

//    float2 ParticleSize; // パーティクルの大きさ
//    float dummy; // ダミー

//}
//cbuffer OBJECT_CONSTANT_BUFFER : register(b1)
//{
//    row_major float4x4 world;
//}

//cbuffer CS_CONSTANT : register(b3)
//{
//    float3 Size;
//    float dummy1;
//}

Texture2D Tex2D : register(t0);; // テクスチャ
SamplerState smpWrap : register(s0); // テクスチャ・サンプラ

cbuffer global
{
    matrix View; // ビュー変換行列
    matrix Projection; // 透視変換行列
    float2 ParticleSize; // パーティクルの大きさ
    int ParticleNo; //大きさ
    float dummy; // ダミー

    float4 EyePos; //カメラ座標
    float4 FogColor; //霧の色
}
//static float3 Acceleration = float3(0.0, -4.8, 0.0);  // 加速度(m/s^2)
													  //static float3 Acceleration = float3(0.0, -9.8, 0.0);  // 加速度(m/s^2)
// VSシェーダの入力データ定義
struct GS_INPUT_GPU2
{
    float3 Position : POSITION; // 頂点座標(ワールド座標系)
    float3 Velocity : VELOCITY; // 現在の速度ベクトル(ワールド座標系)
    float3 Force : FORCE; // 現在の加速度
};

// GSシェーダの入力データ定義
struct GS_INPUT
{
    float4 Pos : SV_POSITION; // 頂点座標(射影座標系)
    float2 Tex : TEXTURE; // テクスチャ座標

};


// ピクセル・シェーダの入力データ定義
struct PS_INPUT
{
    float4 Pos : SV_POSITION; // 頂点座標(射影座標系)
    float2 Tex : TEXTURE; // テクスチャ座標
    float3 wPosition : TEXCOORD2;
};

