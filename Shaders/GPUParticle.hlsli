
// �����̏��ԂƂ��Ă͒��_�V�F�[�_���s�N�Z���V�F�[�_

struct VS_IN
{
    float3 position : POSITION; // ���[�J���ʒu
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION; // ���[���h�ʒu
    float4 world_normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;

};

cbuffer SCENE_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;
}
cbuffer OBJECT_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 world;
}

cbuffer CS_CONSTANT : register(b3)
{
    float3 Size;
    float dummy;
}
