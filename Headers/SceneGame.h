#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "SkyBox.h"
#include "GPUParticle.h"
#include "Font.h"

class SceneGame :public SceneBase {

	// �ϐ� //
private:
	// �V�[�� �h�����p�[�e�B�N��
	std::unique_ptr<GPUParticle> GpuParticle;

	// �V���h�E�}�b�v�֘A
	std::unique_ptr<Sprite> ShadowMapDrawer; // �V���h�E�}�b�v�`��p
	std::unique_ptr<Texture> ShadowMapTexture[2];	// �V���h�E�}�b�v�������ݐ�[0]:�ʏ� [1]:�K�E�X�u���[������
	std::unique_ptr<Texture> GaussShadowMapTexture;	// �K�E�X�u���[������V���h�E�}�b�v
	inline constexpr static float SHADOW_SIZE = 2048;
	std::unique_ptr<Texture> ShadowDepth;	// �V���h�E�}�b�v�p�[�x�������ݐ�H
	std::unique_ptr<ShaderEx> ShadowShader;	// �V���h�E�}�b�v�ɏ������܂���V�F�[�_�[
	std::unique_ptr<ShaderEx> RenderShadowShader;	// �V���h�E�}�b�v�K�p�V�F�[�_�[
	// �V���h�E�}�b�v�p�K�E�X�u���[�֘A
	std::unique_ptr<ShaderEx> GaussianBlur;	// �V���h�E�}�b�v�p�K�E�X�����V�F�[�_
	static constexpr u_int KARNEL_SIZE = 5;
	static constexpr float PI = 3.141592f;
	static constexpr u_int BUFFER_SIZE = KARNEL_SIZE * KARNEL_SIZE;
	struct GaussianBlurConstants {
		DirectX::SimpleMath::Vector4 weight[BUFFER_SIZE]; //�K�E�V�A���t�B���^ X,Y �I�t�Z�b�g�AZ �d��
		float karnel_size;
		DirectX::SimpleMath::Vector2 texcel;
		float dummy;
	};

	struct ShadowConstants {
		DirectX::SimpleMath::Matrix light_view_projection;
		DirectX::SimpleMath::Vector4 light_position;
		DirectX::SimpleMath::Vector4 shadow_color;
	}ShadowConstant;

	// Bloom�G�t�F�N�g�����p�ϐ�
	std::unique_ptr<Texture> BloomDepth;	// �V���h�E�}�b�v�p�[�x�������ݐ�H
	std::unique_ptr<ShaderEx> BloomShader;	// �V���h�E�}�b�v�ɏ������܂���V�F�[�_�[


public:
	// Sprite�^ �摜�`��p
	std::unique_ptr<Sprite> Sprites;
	std::unique_ptr<SkyBox> Skybox;

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

	// �v���C���[�I�u�W�F�N�g
	std::unique_ptr<Player> player;

	// �G�͌���V���O���g���ɂ��Ă���̂ŃN���X���璼�ɌĂяo���Ă���

	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-2.0f,1.0f };	// ���C�g�̌���������
	float ortho = 50.0f;	// ImGui�p�@�ꎞ�I
	bool lightForCamera = false;	// �J�������烉�C�g�𔭎˂��邩
	bool renderShadowMap = false;	// �V���h�E�}�b�v��\�����邩
	int  shadowNo = 0;				// �\������V���h�E�}�b�v�̔ԍ� 0/1
	DirectX::SimpleMath::Vector3 eyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu

	// �J�����Ǘ��N���X�I�Ɏ������Ă� �����Ǘ��c����c
	//std::unique_ptr<Camera> camera = nullptr;
	// instance�g���Ăǂ�����ł��Ăяo����悤�ɕύX�A�s���s�s����������Ζ߂���

	// �֐� //
private:
	void DepthShadowMapping();	// �[�x�l��p�����V���h�E�}�b�s���O
	void GaussianFilter(DirectX::SimpleMath::Vector4* array, int karnel_size, float sigma);

public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�
};

