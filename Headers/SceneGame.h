#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "SkyBox.h"
#include "GPUParticle.h"
#include "Font.h"

class SceneGame :public SceneBase {

	// 変数 //
private:
	// シーン 揺蕩うパーティクル
	std::unique_ptr<GPUParticle> GpuParticle;

	// シャドウマップ関連
	std::unique_ptr<Sprite> ShadowMapDrawer; // シャドウマップ描画用
	std::unique_ptr<Texture> ShadowMapTexture[2];	// シャドウマップ書き込み先[0]:通常 [1]:ガウスブラー処理後
	std::unique_ptr<Texture> GaussShadowMapTexture;	// ガウスブラー処理後シャドウマップ
	inline constexpr static float SHADOW_SIZE = 2048;
	std::unique_ptr<Texture> ShadowDepth;	// シャドウマップ用深度書き込み先？
	std::unique_ptr<ShaderEx> ShadowShader;	// シャドウマップに書き込ませるシェーダー
	std::unique_ptr<ShaderEx> RenderShadowShader;	// シャドウマップ適用シェーダー
	// シャドウマップ用ガウスブラー関連
	std::unique_ptr<ShaderEx> GaussianBlur;	// シャドウマップ用ガウス処理シェーダ
	static constexpr u_int KARNEL_SIZE = 5;
	static constexpr float PI = 3.141592f;
	static constexpr u_int BUFFER_SIZE = KARNEL_SIZE * KARNEL_SIZE;
	struct GaussianBlurConstants {
		DirectX::SimpleMath::Vector4 weight[BUFFER_SIZE]; //ガウシアンフィルタ X,Y オフセット、Z 重み
		float karnel_size;
		DirectX::SimpleMath::Vector2 texcel;
		float dummy;
	};

	struct ShadowConstants {
		DirectX::SimpleMath::Matrix light_view_projection;
		DirectX::SimpleMath::Vector4 light_position;
		DirectX::SimpleMath::Vector4 shadow_color;
	}ShadowConstant;

	// Bloomエフェクト実装用変数
	std::unique_ptr<Texture> BloomDepth;	// シャドウマップ用深度書き込み先？
	std::unique_ptr<ShaderEx> BloomShader;	// シャドウマップに書き込ませるシェーダー


public:
	// Sprite型 画像描画用
	std::unique_ptr<Sprite> Sprites;
	std::unique_ptr<SkyBox> Skybox;

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

	// Geometric_primitiveの変数やつ
	std::unique_ptr< Geometric_Cube> Grid;	// グリッド線もどき

	// プレイヤーオブジェクト
	std::unique_ptr<Player> player;

	// 敵は現状シングルトンにしているのでクラスから直に呼び出している

	// 個人 ImGuiで数値を編集、格納して関数に渡す変数
	float light_dir[3]{ 0.5f,-2.0f,1.0f };	// ライトの向かう方向
	float ortho = 50.0f;	// ImGui用　一時的
	bool lightForCamera = false;	// カメラからライトを発射するか
	bool renderShadowMap = false;	// シャドウマップを表示するか
	int  shadowNo = 0;				// 表示するシャドウマップの番号 0/1
	DirectX::SimpleMath::Vector3 eyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// カメラの位置

	// カメラ管理クラス的に実装してる 多分管理…うん…
	//std::unique_ptr<Camera> camera = nullptr;
	// instance使ってどこからでも呼び出せるように変更、不具合や不都合が分かれば戻そう

	// 関数 //
private:
	void DepthShadowMapping();	// 深度値を用いたシャドウマッピング
	void GaussianFilter(DirectX::SimpleMath::Vector4* array, int karnel_size, float sigma);

public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene内で表示させるimguiを管理
};

