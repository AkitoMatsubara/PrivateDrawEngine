#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "Enemy.h"
#include "Stage.h"

class SceneTest_2 :public SceneBase {

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
		DirectX::XMFLOAT3 dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;                      // ���͗p�̍\�����o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr;                      // �o�͗p�̍\�����o�b�t�@�[

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // ���͗p�̍\�����o�b�t�@�[����쐬�����V�F�[�_�[���\�[�X�r���[
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // �o�͗p�̍\�����o�b�t�@�[����쐬�����A���I�[�_�[�h �A�N�Z�X �r���[

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // ���͗p�o�b�t�@�[�̔z���錾

	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
	ID3D11Buffer* CreateAndCopyToBuffer(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer);
	// �R���s���[�g�V�F�[�_�[�����s����
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pRscSRV, ID3D11UnorderedAccessView* pDstUAV, UINT X, UINT Y, UINT Z);
	// �Ă��Ƃ���---------------------------------------------------------------------

public:
	// Sprite�^ �摜�`��p
	std::unique_ptr<Sprite> sprites;
	std::unique_ptr<sprite_Batch> sprite_batches[8];
	std::unique_ptr<ShaderEx> SpriteShader = nullptr;

	// �V�[���萔�o�b�t�@
	struct scene_constants {
		DirectX::XMFLOAT4X4 view_projection;	// VP�ϊ��s��
		DirectX::XMFLOAT4 light_direction;	// ���C�g�̌���
		DirectX::XMFLOAT4 camera_position;	// �J�����̈ʒu
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitive�̕ϐ����
	std::unique_ptr< Geometric_Cube> grid;	// �O���b�h�����ǂ�
	std::unique_ptr<ShaderEx> GeomtricShader = nullptr;

	// Skkined_Mesh�p
	std::unique_ptr<Skinned_Mesh> skinned_mesh;
	std::unique_ptr<ShaderEx> SkinnedShader = nullptr;

	// �v���C���[�I�u�W�F�N�g
	std::unique_ptr<Player> player;

	// �G
	std::unique_ptr<Enemy> enemy;

	// �X�e�[�W;
	std::unique_ptr<Stage>stage;

	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-2.0f,1.0f };	// ���C�g�̌���������
	bool focus_zero = false;	// �œ_��(0,0,0)�Ɍ����邩�ǂ���
	DirectX::XMFLOAT3 eyePos = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu


	// �֐� //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�
};

