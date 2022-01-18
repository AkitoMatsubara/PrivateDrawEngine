#pragma once
#include <memory>
#include "shaderEx.h"
#include "skinned_mesh.h"
#include "texture.h"
#include "sprite_Batch.h"
#include  "Object3d.h"

#include <SimpleMath.h>

class GPUParticle
{
private:

	// �����ōl�����K�v�����ȕϐ�-----------------------------------------------------
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;	// ���t���[���X�V�̂��߂ɑ��邠��
	Microsoft::WRL::ComPtr<ID3D11Buffer> obj_ConstantBuffer;// �I�u�W�F�N�g�̃��[���h�s��𑗂邽�߂̃o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> cs_ConstantBuffer;	// CS�ɑ���R���X�^���g�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> ReadBackBuffer;	// GPU����ǂݍ��ނ��߂̃o�b�t�@
	std::unique_ptr<Skinned_Mesh> obj;
	std::unique_ptr<sprite_Batch> sprite[1080];
	std::unique_ptr<Object3d> Parameters;	// �p�����[�^
	std::unique_ptr<Texture> texture;

	// �����ōl�����K�v�����ȕϐ�����-------------------------------------------------

	// �Ă���-------------------------------------------------------------------------
	std::unique_ptr<ShaderEx> ParticleShader;

	const static UINT NUM_ELEMENTS = 128;

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
		DirectX::SimpleMath::Vector4 light_direction;	// ���C�g�̌���
		DirectX::SimpleMath::Vector4 camera_position;	// �J�����̈ʒu
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
	void Draw();

	void SetSceneConstantBuffer(const ID3D11Buffer* cbBuf);

	void CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size);

};