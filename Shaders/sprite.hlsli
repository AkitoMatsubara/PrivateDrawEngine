
// �����̏��ԂƂ��Ă͒��_�V�F�[�_���s�N�Z���V�F�[�_

struct VS_IN
{
    float3 position : POSITION; // ���[�J���ʒu
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
};

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;

};