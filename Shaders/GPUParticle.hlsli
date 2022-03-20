
cbuffer global
{
    matrix View;         // ビュー変換行列
    matrix Projection;   // 透視変換行列
    float2 ParticleSize; // パーティクルの大きさ
    int    ParticleNo;   //cpu側からの命令数
    float  dummy;        // ダミー

    float4 EyePos;       //カメラ座標
}

// VSシェーダの入力データ定義
struct VS_INPUT_GPU
{
    float3 Position : POSITION; // 頂点座標(ワールド座標系)
    float3 Velocity : VELOCITY; // 現在の速度ベクトル(ワールド座標系)
    float3 Force    : FORCE;    // 現在の加速度
    float4 Color    : COLOR;
    bool Active     : ACTIVE;
    float  Life     : LIFE;
};

// GSシェーダの入力データ定義
struct GS_INPUT
{
    float4 Pos : SV_POSITION; // 頂点座標(射影座標系)
    float2 Tex : TEXTURE;     // テクスチャ座標
    float4 Color : COLOR;
    float Life : LIFE;
};

// ピクセル・シェーダの入力データ定義
struct PS_INPUT
{
    float4 Pos       : SV_POSITION; // 頂点座標(射影座標系)
    float2 Tex       : TEXTURE;     // テクスチャ座標
    float3 wPosition : TEXCOORD2;
    float4 Color : COLOR;
    float Life : LIFE;
};

