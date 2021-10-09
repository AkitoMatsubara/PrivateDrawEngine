#pragma once
#include "SceneManager.h"

class SceneLoading:public SceneBase{
	// �ϐ�
private:
	unique_ptr<SceneBase> nextScene;

	unique_ptr<Sprite> loadingImage;
	unique_ptr<ShaderEx> SpriteShader = nullptr;
	float angle = 0.0f;

	// �V�[���萔�o�b�t�@
	struct scene_constants {
		XMFLOAT4X4 view_projection;	// VP�ϊ��s��
		XMFLOAT4 light_direction;	// ���C�g�̌���
		XMFLOAT4 camera_position;	// �J�����̈ʒu
	};
	ComPtr<ID3D11Buffer> constant_buffer[8];

	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ���C�g�̌���������
	bool focus_zero = false;	// �œ_��(0,0,0)�Ɍ����邩�ǂ���
	XMFLOAT3 eyePos = XMFLOAT3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu

public:
protected:

	// �֐�
private:
	// ���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);
public:
	SceneLoading() {}
	SceneLoading(std::unique_ptr<SceneBase>nextScene) { this->nextScene = move(nextScene); }
	~SceneLoading() {}

	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�
protected:
};
