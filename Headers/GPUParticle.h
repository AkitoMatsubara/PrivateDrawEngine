#pragma once
#include <memory>
#include "shaderEx.h"
#include "skinned_mesh.h"
#include "texture.h"
#include "sprite_Batch.h"
#include  "Object3d.h"
#include "Camera.h"
#include "Sampler.h"

#include <SimpleMath.h>

class GPUParticle
{
private:

	// 自分で考えた必要そうな変数-----------------------------------------------------
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;	// 毎フレーム更新のために送るあれ
	Microsoft::WRL::ComPtr<ID3D11Buffer> ReadBackBuffer;	// GPUから読み込むためのバッファ
	std::unique_ptr<Object3d> Parameters;	// パラメータ
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Sampler> sample;

	// 自分で考えた必要そうな変数しめ-------------------------------------------------

	// てすと-------------------------------------------------------------------------
	std::unique_ptr<ShaderEx> ParticleShader;

	const static UINT NUM_ELEMENTS = 128;
	int DispathNo;
	int ParticleAmount;
	Microsoft::WRL::ComPtr<ID3D11Buffer> VerticesBuffer;	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> DynamicCBuffer;	// CPUで書き換えることが可能な定数バッファ
	Microsoft::WRL::ComPtr <ID3D11ComputeShader> g_pComputeShader2;  // コンピュート・シェーダ
	Microsoft::WRL::ComPtr<ID3D11Buffer> InputBuffer  = NULL; // バッファ リソース
	Microsoft::WRL::ComPtr<ID3D11Buffer> OutputBuffer = NULL; // バッファ リソース
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  g_pSRV = NULL; // シェーダ リソース ビュー
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> g_pUAV = NULL; // アンオーダード アクセス ビュー

	Microsoft::WRL::ComPtr<ID3D11Buffer> ToCpuBuffer = NULL; // CPUへの書き込み用バッファ リソース
	Microsoft::WRL::ComPtr<ID3D11Buffer> CPUReadBackBuffer = NULL; // リードバック用バッファ リソース


	// 頂点構造体 InputLayoutにも定義して頂点バッファを通して渡す
	struct VBuffer
	{
		DirectX::SimpleMath::Vector3 Position; // 座標値
		DirectX::SimpleMath::Vector3 Velocity; // 速度
		DirectX::SimpleMath::Vector3 Force;    // 加速度
		DirectX::SimpleMath::Vector4 Color;    // 色
		bool Active;
		float Life;
	};
	std::vector<VBuffer> vVecBuf;

	struct cbCBuffer {
		DirectX::SimpleMath::Matrix View;           // ビュー変換行列
		DirectX::SimpleMath::Matrix Projection;     // 透視変換行列
		DirectX::SimpleMath::Vector2 ParticleSize;   // パーティクルの大きさ
		INT32      No;             //
		FLOAT      dummy;          // ダミー
		DirectX::SimpleMath::Vector4	EyePos;  //カメラ座標
	};

	struct ReturnBuffer
	{
		INT32 No;  //一番多い場所
		DirectX::SimpleMath::Vector3 Position; // 座標値
	};
	VBuffer CpuGpuBuffer;	// CSから返ってくる構造体

	// 定数バッファのデータ
	struct cbCBuffer g_cbCBuffer;

	// シーン定数バッファ
	struct scene_constants {
		DirectX::SimpleMath::Matrix view_projection;	// VP変換行列
		DirectX::SimpleMath::Matrix View;
		DirectX::SimpleMath::Matrix Projection;

		DirectX::SimpleMath::Vector4 light_direction;	// ライトの向き
		DirectX::SimpleMath::Vector4 camera_position;	// カメラの位置

		DirectX::SimpleMath::Vector2 ParticleSize;
		int ParticleNo;
		float dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;  // 入力用の構造化バッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr; // 出力用の構造化バッファー

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // 入力用の構造化バッファーから作成されるシェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // 出力用の構造化バッファーから作成されるアンオーダード アクセス ビュー

	//BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // 入力用バッファーの配列を宣言

	// てすとしめ---------------------------------------------------------------------

	void CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size, bool dynamicFlg = false);

public:

	bool Init();
	void Update(Camera* camera);
	void Draw();

	void SetParticle();

	void SetSceneConstantBuffer(const ID3D11Buffer* cbBuf);

	// TODO Debug用
	void ImguiParticles();
	float testColor[4] = { 1.0f,1.0f,1.0f,1.0f };
	float testLife;
	bool runCS = true;
};