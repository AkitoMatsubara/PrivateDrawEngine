#pragma once
#include "Scenemanager.h"

class SceneTitle :public SceneBase {

	// �ϐ� //
private:
public:
	// Sprite�^ �摜�`��p
	std::unique_ptr<Sprite> TitleImage;
	std::unique_ptr<sprite_Batch> sprite_batches[8];
	std::unique_ptr<ShaderEx> SpriteShader = nullptr;


	// �V�[���萔�o�b�t�@
	struct scene_constants {
		DirectX::SimpleMath::Matrix view_projection;	// VP�ϊ��s��
		DirectX::SimpleMath::Vector4 light_direction;	// ���C�g�̌���
		DirectX::SimpleMath::Vector4 camera_position;	// �J�����̈ʒu
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitive�̕ϐ����
	std::unique_ptr< Geometric_Cube> grid;	// �O���b�h�����ǂ�
	std::unique_ptr<ShaderEx> GeomtricShader = nullptr;

	// Skkined_Mesh�p
	std::unique_ptr<Skinned_Mesh> skinned_mesh;
	std::unique_ptr<ShaderEx> SkinnedShader = nullptr;


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
