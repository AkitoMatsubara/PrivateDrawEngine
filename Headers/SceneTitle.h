#pragma once
#include "Scenemanager.h"
#include "GPUParticle.h"

class SceneTitle :public SceneBase {

	// 変数 //
private:
public:
	// Sprite型 画像描画用
	std::unique_ptr<Sprite> TitleImage;
	std::unique_ptr<ShaderEx> SpriteShader = nullptr;


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

	// Skkined_Mesh用
	std::unique_ptr<Skinned_Mesh> skinned_mesh;
	// シーン 揺蕩うパーティクル
	std::unique_ptr<GPUParticle> GpuParticle;


	// 個人 ImGuiで数値を編集、格納して関数に渡す変数
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ライトの向かう方向
	bool focus_zero = true;	// 焦点が(0,0,0)に向けるかどうか
	DirectX::SimpleMath::Vector3 eyePos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f);	// カメラの位置

	// 関数 //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene内で表示させるimguiを管理

};
