#pragma once
#include <memory>
#include "shaderEx.h"
#include "skinned_mesh.h"
#include "texture.h"
#include "sprite_Batch.h"
#include  "Object3d.h"
#include "Camera.h"
#include "Sampler.h"

#include <SimpleMath.h>

class GPUParticle
{
private:

	// �����ōl�����K�v�����ȕϐ�-----------------------------------------------------
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;	// ���t���[���X�V�̂��߂ɑ��邠��
	Microsoft::WRL::ComPtr<ID3D11Buffer> obj_ConstantBuffer;// �I�u�W�F�N�g�̃��[���h�s��𑗂邽�߂̃o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> cs_ConstantBuffer;	// CS�ɑ���R���X�^���g�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> ReadBackBuffer;	// GPU����ǂݍ��ނ��߂̃o�b�t�@
	std::unique_ptr<Object3d> Parameters;	// �p�����[�^
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Sampler> sample;

	// �����ōl�����K�v�����ȕϐ�����-------------------------------------------------

	// �Ă���-------------------------------------------------------------------------
	std::unique_ptr<ShaderEx> ParticleShader;

	const static UINT NUM_ELEMENTS = 128;
	int DispathNo;
	int PerticleAmount;
	int chainA = 0;//�o�b�t�@�[�̐؂�ւ�
	int chainB = 1;//�o�b�t�@�[�̐؂�ւ�
	Microsoft::WRL::ComPtr<ID3D11BlendState> bd_states;
	Microsoft::WRL::ComPtr<ID3D11Buffer> VerticesBuffer;	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> DynamicCBuffer;	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr <ID3D11ComputeShader> g_pComputeShader2;  // �R���s���[�g�E�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11Buffer> InBuffer  = NULL; // �o�b�t�@ ���\�[�X
	Microsoft::WRL::ComPtr<ID3D11Buffer> OutBuffer = NULL; // �o�b�t�@ ���\�[�X
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  g_pSRV[2] = { NULL, NULL }; // �V�F�[�_ ���\�[�X �r���[
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> g_pUAV[2] = { NULL, NULL }; // �A���I�[�_�[�h �A�N�Z�X �r���[

	Microsoft::WRL::ComPtr<ID3D11Buffer> ToCpuBuffer = NULL; // CPU�ւ̏������ݗp�o�b�t�@ ���\�[�X
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> ToCpuUAV = NULL;// �A���I�[�_�[�h �A�N�Z�X �r���[
	Microsoft::WRL::ComPtr<ID3D11Buffer> CPUReadBackBuffer = NULL; // ���[�h�o�b�N�p�o�b�t�@ ���\�[�X


// ���_�\����
	struct VBuffer
	{
		DirectX::SimpleMath::Vector3 Position; // ���W�l
		DirectX::SimpleMath::Vector3 Velocity; // ���x
		DirectX::SimpleMath::Vector3 Force;    // �����x
	};

	struct cbCBuffer {
		DirectX::SimpleMath::Matrix View;           // �r���[�ϊ��s��
		DirectX::SimpleMath::Matrix Projection;     // �����ϊ��s��
		DirectX::SimpleMath::Vector2 ParticleSize;   // �p�[�e�B�N���̑傫��
		INT32      No;             //
		FLOAT      dummy;          // �_�~�[
		DirectX::SimpleMath::Vector4	EyePos;  //�J�������W
		DirectX::SimpleMath::Vector4	FogColor; //���̐F
	};

	struct ReturnBuffer
	{
		INT32 No;  //��ԑ����ꏊ
		DirectX::SimpleMath::Vector3 Position; // ���W�l
	};
	ReturnBuffer CpuGpuBuffer;	// CS����Ԃ��Ă���\����

	// �萔�o�b�t�@�̃f�[�^
	struct cbCBuffer g_cbCBuffer;


	struct BUFIN_TYPE
	{
		DirectX::SimpleMath::Vector3 Position;	// �v�Z�O�ʒu
		float dummy;
	};

	struct BUFOUT_TYPE
	{
		DirectX::SimpleMath::Vector3 Position; // �v�Z��ʒu
		float dummy;
		DirectX::SimpleMath::Vector3 Size;	// �傫���ς��Ă݂悤����
		float dummy2;
	};

	// �V�[���萔�o�b�t�@
	struct scene_constants {
		DirectX::SimpleMath::Matrix view_projection;	// VP�ϊ��s��
		DirectX::SimpleMath::Matrix View;
		DirectX::SimpleMath::Matrix Projection;

		DirectX::SimpleMath::Vector4 light_direction;	// ���C�g�̌���
		DirectX::SimpleMath::Vector4 camera_position;	// �J�����̈ʒu

		DirectX::SimpleMath::Vector2 ParticleSize;
		int ParticleNo;
		float dummy;
	};
	// �I�u�W�F�N�g�R���X�^���g�o�b�t�@
	struct obj_constants
	{
		DirectX::SimpleMath::Matrix world;	// ���[���h�s��
	};
	// CS�R���X�^���g�o�b�t�@
	struct cs_constants {
		DirectX::SimpleMath::Vector3 Size;	// �傫���ς��Ă݂悤����
		float dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;  // ���͗p�̍\�����o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr; // �o�͗p�̍\�����o�b�t�@�[

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // ���͗p�̍\�����o�b�t�@�[����쐬�����V�F�[�_�[���\�[�X�r���[
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // �o�͗p�̍\�����o�b�t�@�[����쐬�����A���I�[�_�[�h �A�N�Z�X �r���[

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // ���͗p�o�b�t�@�[�̔z���錾

	// �Ă��Ƃ���---------------------------------------------------------------------
public:
	bool Init();
	void Update();
	void Draw(Camera* camera);

	void SetSceneConstantBuffer(const ID3D11Buffer* cbBuf);

	void CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size, bool dynamicFlg = false);

};