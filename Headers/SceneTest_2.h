#pragma once
#include "SceneManager.h"

#include "Player.h"
#include "Enemy.h"
#include "Stage.h"

class SceneTest_2 :public SceneBase {

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
		DirectX::XMFLOAT3 dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;                      // 入力用の構造化バッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr;                      // 出力用の構造化バッファー

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // 入力用の構造化バッファーから作成されるシェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // 出力用の構造化バッファーから作成されるアンオーダード アクセス ビュー

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // 入力用バッファーの配列を宣言

	HRESULT CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut);
	// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
	HRESULT CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut);
	// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
	ID3D11Buffer* CreateAndCopyToBuffer(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer);
	// コンピュートシェーダーを実行する
	void RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pRscSRV, ID3D11UnorderedAccessView* pDstUAV, UINT X, UINT Y, UINT Z);
	// てすとしめ---------------------------------------------------------------------

public:
	// Sprite型 画像描画用
	std::unique_ptr<Sprite> sprites;
	std::unique_ptr<sprite_Batch> sprite_batches[8];
	std::unique_ptr<ShaderEx> SpriteShader = nullptr;

	// シーン定数バッファ
	struct scene_constants {
		DirectX::XMFLOAT4X4 view_projection;	// VP変換行列
		DirectX::XMFLOAT4 light_direction;	// ライトの向き
		DirectX::XMFLOAT4 camera_position;	// カメラの位置
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer[8];

	// Geometric_primitiveの変数やつ
	std::unique_ptr< Geometric_Cube> grid;	// グリッド線もどき
	std::unique_ptr<ShaderEx> GeomtricShader = nullptr;

	// Skkined_Mesh用
	std::unique_ptr<Skinned_Mesh> skinned_mesh;
	std::unique_ptr<ShaderEx> SkinnedShader = nullptr;

	// プレイヤーオブジェクト
	std::unique_ptr<Player> player;

	// 敵
	std::unique_ptr<Enemy> enemy;

	// ステージ;
	std::unique_ptr<Stage>stage;

	// 個人 ImGuiで数値を編集、格納して関数に渡す変数
	float light_dir[3]{ 0.5f,-2.0f,1.0f };	// ライトの向かう方向
	bool focus_zero = false;	// 焦点が(0,0,0)に向けるかどうか
	DirectX::XMFLOAT3 eyePos = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);	// カメラの位置


	// 関数 //
private:
public:
	bool Initialize();
	void Update();
	void Render();

	void imguiUpdate();	// Scene内で表示させるimguiを管理
};

