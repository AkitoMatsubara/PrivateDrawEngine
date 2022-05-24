#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "GPUParticle.h"

class SceneClear :public SceneBase {

	// �ϐ� //
private:

public:
	// Sprite�^ �摜�`��p
	std::unique_ptr<Sprite> Sprites;
	std::unique_ptr<sprite_Batch> sprite_batches[8];

	// �V�[���萔�o�b�t�@
	struct SceneConstants {
		DirectX::SimpleMath::Matrix view_projection;	// VP�ϊ��s��
		DirectX::SimpleMath::Vector4 light_direction;	// ���C�g�̌���
		DirectX::SimpleMath::Vector4 camera_position;	// �J�����̈ʒu
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffers[8];

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

