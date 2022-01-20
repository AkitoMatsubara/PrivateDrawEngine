#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "GPUParticle.h"

class SceneGame :public SceneBase {

	// �ϐ� //
private:
	// �Ă���-------------------------------------------------------------------------
	std::unique_ptr<ShaderEx> ComputeShader;


	const static UINT NUM_ELEMENTS = 128;

	struct BUFIN_TYPE
	{
		int i;
		float f;
	};

	struct BUFOUT_TYPE
	{
		float i;
	};

	// �V�[���萔�o�b�t�@
	struct cs_constants {
		float Theta;	// sin�J�[�u�p
		DirectX::SimpleMath::Vector3 dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;                      // ���͗p�̍\�����o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr;                      // �o�͗p�̍\�����o�b�t�@�[

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // ���͗p�̍\�����o�b�t�@�[����쐬�����V�F�[�_�[���\�[�X�r���[
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // �o�͗p�̍\�����o�b�t�@�[����쐬�����A���I�[�_�[�h �A�N�Z�X �r���[

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // ���͗p�o�b�t�@�[�̔z���錾

	std::unique_ptr<GPUParticle> gpu_particle_;
	// �Ă��Ƃ���---------------------------------------------------------------------

public:
	// Sprite�^ �摜�`��p
	std::unique_ptr<Sprite> sprites;
	std::unique_ptr<sprite_Batch> sprite_batches[8];

	// �V�[���萔�o�b�t�@
	struct scene_constants {
		DirectX::SimpleMath::Matrix view_projection;	// VP�ϊ��s��
		DirectX::SimpleMath::Vector4 light_direction;	// ���C�g�̌���
		DirectX::SimpleMath::Vector4 camera_position;	// �J�����̈ʒu
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitive�̕ϐ����
	std::unique_ptr< Geometric_Cube> grid;	// �O���b�h�����ǂ�

	// �v���C���[�I�u�W�F�N�g
	std::unique_ptr<Player> player;

	// �G�͌���V���O���g���ɂ��Ă���̂ŃN���X���璼�ɌĂяo���Ă���

	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-2.0f,1.0f };	// ���C�g�̌���������
	bool focus_zero = false;	// �œ_��(0,0,0)�Ɍ����邩�ǂ���
	DirectX::SimpleMath::Vector3 eyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu

	// �J�����Ǘ��N���X�I�Ɏ������Ă� �����Ǘ��c����c
	std::unique_ptr<Camera> camera = nullptr;

	// �֐� //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�
};

