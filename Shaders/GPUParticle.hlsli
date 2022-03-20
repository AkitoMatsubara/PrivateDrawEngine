
cbuffer global
{
    matrix View;         // �r���[�ϊ��s��
    matrix Projection;   // �����ϊ��s��
    float2 ParticleSize; // �p�[�e�B�N���̑傫��
    int    ParticleNo;   //cpu������̖��ߐ�
    float  dummy;        // �_�~�[

    float4 EyePos;       //�J�������W
}

// VS�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT_GPU
{
    float3 Position : POSITION; // ���_���W(���[���h���W�n)
    float3 Velocity : VELOCITY; // ���݂̑��x�x�N�g��(���[���h���W�n)
    float3 Force    : FORCE;    // ���݂̉����x
    float4 Color    : COLOR;
    bool Active     : ACTIVE;
    float  Life     : LIFE;
};

// GS�V�F�[�_�̓��̓f�[�^��`
struct GS_INPUT
{
    float4 Pos : SV_POSITION; // ���_���W(�ˉe���W�n)
    float2 Tex : TEXTURE;     // �e�N�X�`�����W
    float4 Color : COLOR;
    float Life : LIFE;
};

// �s�N�Z���E�V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT
{
    float4 Pos       : SV_POSITION; // ���_���W(�ˉe���W�n)
    float2 Tex       : TEXTURE;     // �e�N�X�`�����W
    float3 wPosition : TEXCOORD2;
    float4 Color : COLOR;
    float Life : LIFE;
};

