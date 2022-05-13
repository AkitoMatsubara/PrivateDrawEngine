#pragma once

// ���f���ǂݍ��݁A�`��n
#include "shader.h"
#include "shaderEx.h"
#include "texture.h"
#include "sprite.h"
#include "sprite_Batch.h"
#include "geometric_primitive.h"
#include "skinned_mesh.h"

#include "Camera.h"

#include "Sampler.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "framework.h"

#include <SimpleMath.h>
#include <queue>

class SceneBase {
	// �ϐ�
private:
	bool GameReady = false;

protected:
	std::unique_ptr<SceneBase> NewScene;
	//�R���X�^���g�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;

	std::shared_ptr<Sampler> DefaultSampleClamp;

	FLOAT ClearColor[4] = { 0.2f,0.2f,0.2f,1.0f };	// ��ʂ��܂�����ɖ߂��F

public:
	// �֐�
private:
protected:
	void CreateConstantBuffer(ID3D11Buffer** ppBuffer, u_int size);
public:
	SceneBase() :NewScene(nullptr){}
	virtual ~SceneBase();

	virtual bool Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Render()     = 0;

	void imguiSceneChanger();

	void setScene(std::unique_ptr<SceneBase> scene) { NewScene = std::move(scene); }
	std::unique_ptr<SceneBase> getScene() { return std::move(NewScene); }

	// �����������Ă��邩
	bool isReady() const { return GameReady; }
	// ���������ݒ�
	void setReady(bool set) { GameReady = set; }

};

//----------------------------------------------------
//		ScenManager(�Ǘ�)�N���X
//----------------------------------------------------
//	�N�������s�V�[���w��
//using	SceneFirst = SceneTest;

// �N�������s�V�[����framework.cpp�ɃC���N���[�h���Ďw��B����N���Ɏg���ɂ�framework.cpp�ɃC���N���[�h���Ďw�肷��


class SceneManager{
private:
	std::unique_ptr<SceneBase> CurrentScene;
public:
	static SceneManager& getInstance() {
		static SceneManager instance;
		return instance;
	}


	SceneManager() :CurrentScene(nullptr) {}

	void Update();
	void Render();
	void ChangeScene(SceneBase* newScene);	// �V�[���؂�ւ��֐�
	//void ChangeScene(std::unique_ptr<SceneBase> newScene);	// �V�[���؂�ւ��֐�
};
