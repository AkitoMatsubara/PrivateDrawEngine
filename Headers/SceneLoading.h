#pragma once
#include "SceneManager.h"
#include "GPUParticle.h"

class SceneLoading:public SceneBase{
	// 変数
private:
	std::unique_ptr<SceneBase> NextScene;	// ロードするシーン

	// マルチスレッド用DeferrerdContext
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deferredContext;

	std::unique_ptr<Sprite> LoadingImage;

	// シーン定数バッファ
	struct SceneConstants
	{
		DirectX::SimpleMath::Matrix view_projection;
		DirectX::SimpleMath::Vector4 light_direction;
		DirectX::SimpleMath::Vector4 camera_position;
		DirectX::SimpleMath::Matrix  view;
		DirectX::SimpleMath::Matrix  projection;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffers[8];

	// 個人 ImGuiで数値を編集、格納して関数に渡す変数
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ライトの向かう方向
	bool focus_zero = false;	// 焦点が(0,0,0)に向けるかどうか
	DirectX::SimpleMath::Vector3 EyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// カメラの位置

	std::unique_ptr<GPUParticle> GpuParticle;

public:
protected:

	// 関数
private:
	// ローディングスレッド
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

	void imguiUpdate();	// Scene内で表示させるimguiを管理
protected:
};
