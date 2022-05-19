#pragma once
#include "Scenemanager.h"
#include "GPUParticle.h"

class SceneTitle :public SceneBase {

	// �ϐ� //
private:
public:
	// Sprite�^ �摜�`��p
	std::unique_ptr<Sprite> TitleImage;
	std::unique_ptr<ShaderEx> SpriteShader = nullptr;


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

	// Geometric_primitive�̕ϐ����
	std::unique_ptr< Geometric_Cube> Grid;	// �O���b�h�����ǂ�

	// Skkined_Mesh�p
	std::unique_ptr<Skinned_Mesh> skinned_mesh;
	// �V�[�� �h�����p�[�e�B�N��
	std::unique_ptr<GPUParticle> GpuParticle;


	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ���C�g�̌���������
	bool focus_zero = true;	// �œ_��(0,0,0)�Ɍ����邩�ǂ���
	DirectX::SimpleMath::Vector3 eyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu

	// �֐� //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�

};
