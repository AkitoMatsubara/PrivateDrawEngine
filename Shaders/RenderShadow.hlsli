#include "ReflectiveModelFunc.hlsli"
//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
//cbuffer CBPerFrame2 : register(b2)
//{
//	float4	LightColor;		//���C�g�̐F
//	float4	LightDir;		//���C�g�̕���
//	float4	AmbientColor;	//����
//	float4  EyePos;			//�J�������W
//};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world;
	float4 material_color;
}

//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
	float4 Position : POSITION;
	float4 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};



//�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//���[���h�@��
	float3 wPosition : TEXCOORD2;//���[���h���W
	float2 Tex : TEXCOORD3;
	float3 vShadow : TEXCOORD4;
};