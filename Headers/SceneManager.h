#pragma once

// モデル読み込み、描画系
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
	// 変数
private:
	bool ready = false;

protected:
	std::unique_ptr<SceneBase> newScene;
	//コンスタントバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;

	std::shared_ptr<Sampler> sampleClamp;

	FLOAT ClearColor[4] = { 0.2f,0.2f,0.2f,1.0f };

public:
	// 関数
private:
protected:
	void CreateConstantBuffer(ID3D11Buffer** ppBuffer, u_int size);
public:
	SceneBase() :newScene(nullptr){}
	virtual ~SceneBase();

	virtual bool Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Render()     = 0;

	void imguiSceneChanger();

	void setScene(std::unique_ptr<SceneBase> scene) { newScene = std::move(scene); }
	std::unique_ptr<SceneBase> getScene() { return std::move(newScene); }

	// 準備完了しているか
	bool isReady() const { return ready; }
	// 準備完了設定
	void setReady(bool set) { ready = set; }

};

//----------------------------------------------------
//		ScenManager(管理)クラス
//----------------------------------------------------
//	起動時実行シーン指定
//using	SceneFirst = SceneTest;

// 起動時実行シーンはframework.cppにインクルードして指定。初回起動に使うにはframework.cppにインクルードして指定する


class SceneManager{
private:
	std::unique_ptr<SceneBase> currentScene;
public:
	static SceneManager& getInstance() {
		static SceneManager instance;
		return instance;
	}


	SceneManager() :currentScene(nullptr) {}

	void Update();
	void Render();
	void ChangeScene(SceneBase* newScene);	// シーン切り替え関数
	//void ChangeScene(std::unique_ptr<SceneBase> newScene);	// シーン切り替え関数
};
