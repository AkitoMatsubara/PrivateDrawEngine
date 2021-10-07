#pragma once
#include <DirectXMath.h>
#include <queue>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "framework.h"

#include "shader.h"
#include "shaderEx.h"
#include "texture.h"
#include "sprite.h"
#include "sprite_Batch.h"
#include "geometric_primitive.h"
#include "static_mesh.h"
#include "skinned_mesh.h"

#include "Sampler.h"

class SceneBase {
	// �ϐ�
private:

protected:
	unique_ptr<SceneBase> newScene;
	//�R���X�^���g�o�b�t�@
	ComPtr<ID3D11Buffer> ConstantBuffer;

	//DepthStencilState
	enum { DS_FALSE, DS_TRUE, DS_FALSE_WRITE, DS_TRUE_WRITE, DS_END };

	shared_ptr<Sampler> sample;

	FLOAT ClearColor[4] = { 0.2f,0.2f,0.2f,1.0f };

public:

	// �֐�
private:
protected:
public:
	SceneBase() :newScene(nullptr){
	}
	virtual ~SceneBase()
	{
		//�X�e�[�g�̏�����
		ID3D11DeviceContext* device_context = framework::getInstance()->GetDeviceContext();
		device_context->ClearState();
		device_context->Flush();

	}

	virtual bool Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Render()     = 0;

	void imguiSceneChanger();
	void setScene(unique_ptr<SceneBase> scene) { newScene = move(scene); }
	unique_ptr<SceneBase> getScene() { return move(newScene); }
};


// ����V�[�� �ŏ��̃V�[���Ƃ��Đݒ肷��K�v�����������߂��̃w�b�_�[�ɋL��
class SceneTest :public SceneBase {

	// �ϐ� //
private:
public:
	// Sprite�^ �摜�`��p
	unique_ptr<Sprite> sprites;
	unique_ptr<sprite_Batch> sprite_batches[8];
	unique_ptr<ShaderEx> SpriteShader = nullptr;


	// �V�[���萔�o�b�t�@
	struct scene_constants {
		XMFLOAT4X4 view_projection;	// VP�ϊ��s��
		XMFLOAT4 light_direction;	// ���C�g�̌���
		XMFLOAT4 camera_position;	// �J�����̈ʒu
	};
	ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitive�̕ϐ����
	unique_ptr< Geometric_Cube> grid;	// �O���b�h�����ǂ�
	unique_ptr<ShaderEx> GeomtricShader = nullptr;

	// Skkined_Mesh�p
	unique_ptr<Skinned_Mesh> skinned_mesh;
	unique_ptr<ShaderEx> SkinnedShader = nullptr;


	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ���C�g�̌���������
	bool focus_zero = true;	// �œ_��(0,0,0)�Ɍ����邩�ǂ���
	XMFLOAT3 eyePos = XMFLOAT3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu

	// �֐� //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�

};

class SceneTest_2 :public SceneBase {

	// �ϐ� //
private:
	// �Ă���
	ComPtr<ID3D11ComputeShader> cs;
	unique_ptr<ShaderEx> ComputeShader;


	const static UINT NUM_ELEMENTS = 128;

	struct BUFIN_TYPE
	{
		int i;
		float f;
	};

	struct BUFOUT_TYPE
	{
		int i;
	};

	ComPtr<ID3D11ComputeShader> pComputeShader = nullptr;      // �R���s���[�g�V�F�[�_�[ �C���^�[�t�F�[�X

	ComPtr<ID3D11Buffer> pBufInput  = nullptr;                      // ���͗p�̍\�����o�b�t�@�[
	ComPtr<ID3D11Buffer> pBufResult = nullptr;                      // �o�͗p�̍\�����o�b�t�@�[

	ComPtr<ID3D11ShaderResourceView>	pBufInputSRV  = nullptr;        // ���͗p�̍\�����o�b�t�@�[����쐬�����V�F�[�_�[���\�[�X�r���[
	ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // �o�͗p�̍\�����o�b�t�@�[����쐬�����A���I�[�_�[�h �A�N�Z�X �r���[

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // ���͗p�o�b�t�@�[�̔z���錾

	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// �R���s���[�g�V�F�[�_�[����̏o�͎��Ɏg�p����A���I�[�_�[�h�A�N�Z�X�r���[���쐬����
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// �A���I�[�_�[�h�A�N�Z�X�r���[�̃o�b�t�@�̓��e�� CPU ����ǂݍ��݉\�ȃo�b�t�@�փR�s�[����
	ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer);

	// �Ă��Ƃ���

public:
	// Sprite�^ �摜�`��p
	unique_ptr<Sprite> sprites;
	unique_ptr<sprite_Batch> sprite_batches[8];
	unique_ptr<ShaderEx> SpriteShader = nullptr;

	// �V�[���萔�o�b�t�@
	struct scene_constants {
		XMFLOAT4X4 view_projection;	// VP�ϊ��s��
		XMFLOAT4 light_direction;	// ���C�g�̌���
		XMFLOAT4 camera_position;	// �J�����̈ʒu
	};
	ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitive�̕ϐ����
	unique_ptr< Geometric_Cube> grid;	// �O���b�h�����ǂ�
	unique_ptr<ShaderEx> GeomtricShader = nullptr;

	// Skkined_Mesh�p
	unique_ptr<Skinned_Mesh> skinned_mesh;
	unique_ptr<ShaderEx> SkinnedShader = nullptr;


	// �l ImGui�Ő��l��ҏW�A�i�[���Ċ֐��ɓn���ϐ�
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ���C�g�̌���������
	bool focus_zero = true;	// �œ_��(0,0,0)�Ɍ����邩�ǂ���
	XMFLOAT3 eyePos = XMFLOAT3(0.0f, 0.0f, -10.0f);	// �J�����̈ʒu


	// �֐� //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene���ŕ\��������imgui���Ǘ�

};

//----------------------------------------------------
//		ScenManager(�Ǘ�)�N���X
//----------------------------------------------------
//	�N�������s�V�[���w��
using	SceneFirst = SceneTest;



class SceneManager{
private:
	unique_ptr<SceneBase> scene;
public:
	SceneManager() :scene(nullptr) {}
	~SceneManager() {}

	void Update();
	void Render();
	void ChangeScene(unique_ptr<SceneBase> newScene);	// �V�[���؂�ւ��֐�
};
