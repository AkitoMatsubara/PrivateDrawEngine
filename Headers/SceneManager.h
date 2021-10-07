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
	// 変数
private:

protected:
	unique_ptr<SceneBase> newScene;
	//コンスタントバッファ
	ComPtr<ID3D11Buffer> ConstantBuffer;

	//DepthStencilState
	enum { DS_FALSE, DS_TRUE, DS_FALSE_WRITE, DS_TRUE_WRITE, DS_END };

	shared_ptr<Sampler> sample;

	FLOAT ClearColor[4] = { 0.2f,0.2f,0.2f,1.0f };

public:

	// 関数
private:
protected:
public:
	SceneBase() :newScene(nullptr){
	}
	virtual ~SceneBase()
	{
		//ステートの初期化
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


// 試作シーン 最初のシーンとして設定する必要があったためこのヘッダーに記載
class SceneTest :public SceneBase {

	// 変数 //
private:
public:
	// Sprite型 画像描画用
	unique_ptr<Sprite> sprites;
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

class SceneTest_2 :public SceneBase {

	// 変数 //
private:
	// てすと
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

	ComPtr<ID3D11ComputeShader> pComputeShader = nullptr;      // コンピュートシェーダー インターフェース

	ComPtr<ID3D11Buffer> pBufInput  = nullptr;                      // 入力用の構造化バッファー
	ComPtr<ID3D11Buffer> pBufResult = nullptr;                      // 出力用の構造化バッファー

	ComPtr<ID3D11ShaderResourceView>	pBufInputSRV  = nullptr;        // 入力用の構造化バッファーから作成されるシェーダーリソースビュー
	ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // 出力用の構造化バッファーから作成されるアンオーダード アクセス ビュー

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // 入力用バッファーの配列を宣言

	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
	ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer);

	// てすとしめ

public:
	// Sprite型 画像描画用
	unique_ptr<Sprite> sprites;
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

//----------------------------------------------------
//		ScenManager(管理)クラス
//----------------------------------------------------
//	起動時実行シーン指定
using	SceneFirst = SceneTest;



class SceneManager{
private:
	unique_ptr<SceneBase> scene;
public:
	SceneManager() :scene(nullptr) {}
	~SceneManager() {}

	void Update();
	void Render();
	void ChangeScene(unique_ptr<SceneBase> newScene);	// シーン切り替え関数
};
