#pragma once
#include "SceneManager.h"

class SceneLoading:public SceneBase{
	// 変数
private:
	std::unique_ptr<SceneBase> nextScene;

	std::unique_ptr<Sprite> loadingImage;
	std::unique_ptr<ShaderEx> SpriteShader = nullptr;
	float angle = 0.0f;

	// シーン定数バッファ
	struct scene_constants {
		DirectX::XMFLOAT4X4 view_projection;	// VP変換行列
		DirectX::XMFLOAT4 light_direction;	// ライトの向き
		DirectX::XMFLOAT4 camera_position;	// カメラの位置
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer[8];

	// 個人 ImGuiで数値を編集、格納して関数に渡す変数
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ライトの向かう方向
	bool focus_zero = false;	// 焦点が(0,0,0)に向けるかどうか
	DirectX::XMFLOAT3 eyePos = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);	// カメラの位置

public:
protected:

	// 関数
private:
	// ローディングスレッド
	static void LoadingThread(SceneLoading* scene);
public:
	SceneLoading() {}
	SceneLoading(std::unique_ptr<SceneBase>nextScene) { this->nextScene = move(nextScene); }
	~SceneLoading() {}

	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene内で表示させるimguiを管理
protected:
};
