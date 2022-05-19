#pragma once
#include "SceneManager.h"
#include "GPUParticle.h"

class SceneLoading:public SceneBase{
	// �ϐ�
private:
	std::unique_ptr<SceneBase> NextScene;	// ���[�h����V�[��

	// �}���`�X���b�h�pDeferrerdContext
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deferredContext;

	std::unique_ptr<Sprite> LoadingImage;

	// �V�[���萔�o�b�t�@
	struct SceneConstants
	{
		DirectX::SimpleMath::Matrix view_projection;
		DirectX::SimpleMath::Vector4 light_direction;
		DirectX::SimpleMath::Vector4 camera_position;
		DirectX::SimpleMath::Matrix  view;
		DirectX::SimpleMath::Matrix  projection;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffers[8];

	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ���C�g�̌���������
	bool focus_zero = false;	// �œ_��(0,0,0)�Ɍ����邩�ǂ���
	DirectX::SimpleMath::Vector3 EyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu

	std::unique_ptr<GPUParticle> GpuParticle;

public:
protected:

	// �֐�
private:
	// ���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);
	std::thread Thread;
public:
	SceneLoading(std::unique_ptr<SceneBase>nextScene) :NextScene(nullptr)
	{
		this->NextScene = std::move(nextScene);
	}
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�
protected:
};
