#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "GPUParticle.h"

class SceneClear :public SceneBase {

	// 変数 //
private:

public:
	// Sprite型 画像描画用
	std::unique_ptr<Sprite> Sprites;
	std::unique_ptr<sprite_Batch> sprite_batches[8];

	// シーン定数バッファ
	struct SceneConstants {
		DirectX::SimpleMath::Matrix view_projection;	// VP変換行列
		DirectX::SimpleMath::Vector4 light_direction;	// ライトの向き
		DirectX::SimpleMath::Vector4 camera_position;	// カメラの位置
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffers[8];

	// プレイヤーオブジェクト
	std::unique_ptr<Player> player;

	// 敵は現状シングルトンにしているのでクラスから直に呼び出している

	// 個人 ImGuiで数値を編集、格納して関数に渡す変数
	float light_dir[3]{ 0.5f,-2.0f,1.0f };	// ライトの向かう方向
	bool focus_zero = false;	// 焦点が(0,0,0)に向けるかどうか
	DirectX::SimpleMath::Vector3 eyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// カメラの位置

	// カメラ管理クラス的に実装してる 多分管理…うん…
	std::unique_ptr<Camera> camera = nullptr;

	// 関数 //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene内で表示させるimguiを管理
};

