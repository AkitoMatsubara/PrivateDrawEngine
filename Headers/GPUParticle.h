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
	Microsoft::WRL::ComPtr<ID3D11Buffer> ReadBackBuffer;	// GPU����ǂݍ��ނ��߂̃o�b�t�@
	std::unique_ptr<Object3d> Parameters;	// �p�����[�^
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Sampler> sample;

	// �����ōl�����K�v�����ȕϐ�����-------------------------------------------------

	// �Ă���-------------------------------------------------------------------------
	std::unique_ptr<ShaderEx> ParticleShader;

	const static UINT NUM_ELEMENTS = 128;
	int DispathNo;
	int ParticleAmount;
	Microsoft::WRL::ComPtr<ID3D11Buffer> VerticesBuffer;	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> DynamicCBuffer;	// CPU�ŏ��������邱�Ƃ��\�Ȓ萔�o�b�t�@
	Microsoft::WRL::ComPtr <ID3D11ComputeShader> g_pComputeShader2;  // �R���s���[�g�E�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11Buffer> InputBuffer  = NULL; // �o�b�t�@ ���\�[�X
	Microsoft::WRL::ComPtr<ID3D11Buffer> OutputBuffer = NULL; // �o�b�t�@ ���\�[�X
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  g_pSRV = NULL; // �V�F�[�_ ���\�[�X �r���[
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> g_pUAV = NULL; // �A���I�[�_�[�h �A�N�Z�X �r���[

	Microsoft::WRL::ComPtr<ID3D11Buffer> ToCpuBuffer = NULL; // CPU�ւ̏������ݗp�o�b�t�@ ���\�[�X
	Microsoft::WRL::ComPtr<ID3D11Buffer> CPUReadBackBuffer = NULL; // ���[�h�o�b�N�p�o�b�t�@ ���\�[�X


	// ���_�\���� InputLayout�ɂ���`���Ē��_�o�b�t�@��ʂ��ēn��
	struct VBuffer
	{
		DirectX::SimpleMath::Vector3 Position; // ���W�l
		DirectX::SimpleMath::Vector3 Velocity; // ���x
		DirectX::SimpleMath::Vector3 Force;    // �����x
		DirectX::SimpleMath::Vector4 Color;    // �F
		bool Active;
		float Life;
	};
	std::vector<VBuffer> vVecBuf;

	struct cbCBuffer {
		DirectX::SimpleMath::Matrix View;           // �r���[�ϊ��s��
		DirectX::SimpleMath::Matrix Projection;     // �����ϊ��s��
		DirectX::SimpleMath::Vector2 ParticleSize;   // �p�[�e�B�N���̑傫��
		INT32      No;             //
		FLOAT      dummy;          // �_�~�[
		DirectX::SimpleMath::Vector4	EyePos;  //�J�������W
	};

	struct ReturnBuffer
	{
		INT32 No;  //��ԑ����ꏊ
		DirectX::SimpleMath::Vector3 Position; // ���W�l
	};
	VBuffer CpuGpuBuffer;	// CS����Ԃ��Ă���\����

	// �萔�o�b�t�@�̃f�[�^
	struct cbCBuffer g_cbCBuffer;

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

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;  // ���͗p�̍\�����o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr; // �o�͗p�̍\�����o�b�t�@�[

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // ���͗p�̍\�����o�b�t�@�[����쐬�����V�F�[�_�[���\�[�X�r���[
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // �o�͗p�̍\�����o�b�t�@�[����쐬�����A���I�[�_�[�h �A�N�Z�X �r���[

	//BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // ���͗p�o�b�t�@�[�̔z���錾

	// �Ă��Ƃ���---------------------------------------------------------------------

	void CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size, bool dynamicFlg = false);

public:

	bool Init();
	void Update(Camera* camera);
	void Draw();

	void SetParticle();

	void SetSceneConstantBuffer(const ID3D11Buffer* cbBuf);

	// TODO Debug�p
	void ImguiParticles();
	float testColor[4] = { 1.0f,1.0f,1.0f,1.0f };
	float testLife;
	bool runCS = true;
};