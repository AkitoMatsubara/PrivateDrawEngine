
//// �����̏��ԂƂ��Ă͒��_�V�F�[�_���s�N�Z���V�F�[�_

//struct VS_IN
//{
//    float3 position : POSITION; // ���[�J���ʒu
//    float3 normal : NORMAL;
//    float2 texcoord : TEXCOORD;
//    float4 color : COLOR;
//};

//struct PS_IN
//{
//    float4 position : SV_POSITION;
//    float4 color : COLOR;
//    float3 world_position : POSITION; // ���[���h�ʒu
//    float2 texcoord : TEXCOORD;
//};

//// VS�V�F�[�_�̓��̓f�[�^��`
//struct GS_INPUT_GPU2
//{
//    float3 position : POSITION; // ���_���W(���[���h���W�n)
//    float3 velocity : VELOCITY; // ���݂̑��x�x�N�g��(���[���h���W�n)
//    float3 force : FORCE; // ���݂̉����x
//};

//// GS�V�F�[�_�̓��̓f�[�^��`
//struct GS_INPUT
//{
//    float4 Pos : SV_POSITION; // ���_���W(�ˉe���W�n)
//    float2 Tex : TEXTURE; // �e�N�X�`�����W

//};
//cbuffer global
//{
//    matrix View; // �r���[�ϊ��s��
//    matrix Projection; // �����ϊ��s��
//    float2 ParticleSize; // �p�[�e�B�N���̑傫��
//    int ParticleNo; //�傫��
//    float dummy; // �_�~�[

//    float4 EyePos; //�J�������W
//    float4 FogColor; //���̐F
//    float FogNear; //���̎n�܂�ʒu
//    float FogFar; //���̏I���ʒu
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

//    float2 ParticleSize; // �p�[�e�B�N���̑傫��
//    float dummy; // �_�~�[

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

Texture2D Tex2D : register(t0);; // �e�N�X�`��
SamplerState smpWrap : register(s0); // �e�N�X�`���E�T���v��

cbuffer global
{
    matrix View; // �r���[�ϊ��s��
    matrix Projection; // �����ϊ��s��
    float2 ParticleSize; // �p�[�e�B�N���̑傫��
    int ParticleNo; //�傫��
    float dummy; // �_�~�[

    float4 EyePos; //�J�������W
    float4 FogColor; //���̐F
}
//static float3 Acceleration = float3(0.0, -4.8, 0.0);  // �����x(m/s^2)
													  //static float3 Acceleration = float3(0.0, -9.8, 0.0);  // �����x(m/s^2)
// VS�V�F�[�_�̓��̓f�[�^��`
struct GS_INPUT_GPU2
{
    float3 Position : POSITION; // ���_���W(���[���h���W�n)
    float3 Velocity : VELOCITY; // ���݂̑��x�x�N�g��(���[���h���W�n)
    float3 Force : FORCE; // ���݂̉����x
};

// GS�V�F�[�_�̓��̓f�[�^��`
struct GS_INPUT
{
    float4 Pos : SV_POSITION; // ���_���W(�ˉe���W�n)
    float2 Tex : TEXTURE; // �e�N�X�`�����W

};


// �s�N�Z���E�V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT
{
    float4 Pos : SV_POSITION; // ���_���W(�ˉe���W�n)
    float2 Tex : TEXTURE; // �e�N�X�`�����W
    float3 wPosition : TEXCOORD2;
};

