#pragma once
#include "Scenemanager.h"

class SceneTitle :public SceneBase {

	// 変数 //
private:
public:
	// Sprite型 画像描画用
	unique_ptr<Sprite> TitleImage;
	unique_ptr<sprite_Batch> sprite_batches[8];
	unique_ptr<ShaderEx> SpriteShader = nullptr;


	// シーン定数バッファ
	struct scene_constants {
		XMFLOAT4X4 view_projection;	// VP変換行列
		XMFLOAT4 light_direction;	// ライトの向き
		XMFLOAT4 camera_position;	// カメラの位置
	};
	ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitiveの変数やつ
	unique_ptr< Geometric_Cube> grid;	// グリッド線もどき
	unique_ptr<ShaderEx> GeomtricShader = nullptr;

	// Skkined_Mesh用
	unique_ptr<Skinned_Mesh> skinned_mesh;
	unique_ptr<ShaderEx> SkinnedShader = nullptr;


	// 個人 ImGuiで数値を編集、格納して関数に渡す変数
	float light_dir[3]{ 0.5f,-1.0f,1.0f };	// ライトの向かう方向
	bool focus_zero = true;	// 焦点が(0,0,0)に向けるかどうか
	XMFLOAT3 eyePos = XMFLOAT3(0.0f, 0.0f, -10.0f);	// カメラの位置

	// 関数 //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene内で表示させるimguiを管理

};
