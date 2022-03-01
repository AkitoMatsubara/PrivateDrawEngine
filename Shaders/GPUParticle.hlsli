
//// 処理の順番としては頂点シェーダ→ピクセルシェーダ


cbuffer global
{
    matrix View;         // ビュー変換行列
    matrix Projection;   // 透視変換行列
    float2 ParticleSize; // パーティクルの大きさ
    int    ParticleNo;   //cpu側からの命令数
    float  dummy;        // ダミー

    float4 EyePos;       //カメラ座標
}
//static float3 Acceleration = float3(0.0, -4.8, 0.0);  // 加速度(m/s^2)

// VSシェーダの入力データ定義
struct GS_INPUT_GPU2
{
    float3 Position : POSITION; // 頂点座標(ワールド座標系)
    float3 Velocity : VELOCITY; // 現在の速度ベクトル(ワールド座標系)
    float3 Force    : FORCE;    // 現在の加速度
};

// GSシェーダの入力データ定義
struct GS_INPUT
{
    float4 Pos : SV_POSITION; // 頂点座標(射影座標系)
    float2 Tex : TEXTURE;     // テクスチャ座標

};

// ピクセル・シェーダの入力データ定義
struct PS_INPUT
{
    float4 Pos       : SV_POSITION; // 頂点座標(射影座標系)
    float2 Tex       : TEXTURE;     // テクスチャ座標
    float3 wPosition : TEXCOORD2;
};

