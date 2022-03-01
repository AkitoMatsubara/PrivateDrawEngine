#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "SkyBox.h"
#include "GPUParticle.h"
#include "Font.h"

class SceneGame :public SceneBase {

	// 変数 //
private:
	// てすと-------------------------------------------------------------------------
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

	// シーン定数バッファ
	struct cs_constants {
		float Theta;	// sinカーブ用
		DirectX::SimpleMath::Vector3 dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;                      // 入力用の構造化バッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr;                      // 出力用の構造化バッファー

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // 入力用の構造化バッファーから作成されるシェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // 出力用の構造化バッファーから作成されるアンオーダード アクセス ビュー

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // 入力用バッファーの配列を宣言

	std::unique_ptr<GPUParticle> GpuParticle;
	// てすとしめ---------------------------------------------------------------------

public:
	// Sprite型 画像描画用
	std::unique_ptr<Sprite> sprites;
	std::unique_ptr<sprite_Batch> sprite_batches[8];
	std::unique_ptr<Font> test;
	std::unique_ptr<SkyBox> skybox;

	// シーン定数バッファ
	//struct scene_constants {
	//	DirectX::SimpleMath::Matrix view_projection;	// VP変換行列
	//	DirectX::SimpleMath::Vector4 light_direction;	// ライトの向き
	//	DirectX::SimpleMath::Vector4 camera_position;	// カメラの位置
	//};

	struct scene_constants
	{
		DirectX::SimpleMath::Matrix view_projection;

		DirectX::SimpleMath::Vector4 light_direction;
		DirectX::SimpleMath::Vector4 camera_position;

		DirectX::SimpleMath::Matrix  View;
		DirectX::SimpleMath::Matrix  Projection;
		DirectX::SimpleMath::Vector2 ParticleSize; // パーティクルの大きさ
		float dummy; // ダミー
		float dummy2;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitiveの変数やつ
	std::unique_ptr< Geometric_Cube> grid;	// グリッド線もどき

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

