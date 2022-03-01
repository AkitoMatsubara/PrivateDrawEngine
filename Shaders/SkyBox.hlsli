
// �����̏��ԂƂ��Ă͒��_�V�F�[�_���s�N�Z���V�F�[�_

struct VS_IN
{
    float3 NDCPosition : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
};

struct PS_IN
{
    float4 NDCPosition      : SV_POSITION; // ���[�J���ʒu
    float3 worldPosition : POSITION;
    float4 color         : COLOR;
};

cbuffer cbScene : register(b0)
{
    float4 viewPosition;
    row_major float4x4 inverseView;
    row_major float4x4 inverseProjection;
}