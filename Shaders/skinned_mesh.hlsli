struct VS_IN
{
    float4 position : POSITION; // ローカル位置
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;   // ワールド位置
    float4 world_normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
}

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;
}

//--------------------------------------------
//	拡散反射関数(ランバート)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率

float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D); // 負の値を０にする
    return C * D * K;
}

//--------------------------------------------
//	鏡面反射関数(フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//反射ベクトル
    float3 R = normalize(reflect(L, N));
    float3 S = dot(R, E);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}