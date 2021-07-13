
struct VS_OUT
{
    float4 poosition : SV_POSITION; // SV_Position：処理されたあとの位置情報的なノリ SV=System Value
    float4 color : COLOR;
};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;   // row_major：行優先
    float4 material_color;
}

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction;
}