#pragma once
#include <memory>
#include "shaderEx.h"
#include "skinned_mesh.h"
#include "texture.h"
#include "sprite_Batch.h"
#include  "Object3d.h"

#include <SimpleMath.h>

class GPUParticle
{
private:

	// 自分で考えた必要そうな変数-----------------------------------------------------
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;	// 毎フレーム更新のために送るあれ
	Microsoft::WRL::ComPtr<ID3D11Buffer> obj_ConstantBuffer;// オブジェクトのワールド行列を送るためのバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> cs_ConstantBuffer;	// CSに送るコンスタントバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> ReadBackBuffer;	// GPUから読み込むためのバッファ
	std::unique_ptr<Skinned_Mesh> obj;
	std::unique_ptr<sprite_Batch> sprite[1080];
	std::unique_ptr<Object3d> Parameters;	// パラメータ
	std::unique_ptr<Texture> texture;

	// 自分で考えた必要そうな変数しめ-------------------------------------------------

	// てすと-------------------------------------------------------------------------
	std::unique_ptr<ShaderEx> ParticleShader;

	const static UINT NUM_ELEMENTS = 128;

	struct BUFIN_TYPE
	{
		DirectX::SimpleMath::Vector3 Position;	// 計算前位置
		float dummy;
	};

	struct BUFOUT_TYPE
	{
		DirectX::SimpleMath::Vector3 Position; // 計算後位置
		float dummy;
		DirectX::SimpleMath::Vector3 Size;	// 大きさ変えてみようかと
		float dummy2;
	};

	// シーン定数バッファ
	struct scene_constants {
		DirectX::SimpleMath::Matrix view_projection;	// VP変換行列
		DirectX::SimpleMath::Vector4 light_direction;	// ライトの向き
		DirectX::SimpleMath::Vector4 camera_position;	// カメラの位置
	};
	// オブジェクトコンスタントバッファ
	struct obj_constants
	{
		DirectX::SimpleMath::Matrix world;	// ワールド行列
	};
	// CSコンスタントバッファ
	struct cs_constants {
		DirectX::SimpleMath::Vector3 Size;	// 大きさ変えてみようかと
		float dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufInput = nullptr;  // 入力用の構造化バッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> pBufResult = nullptr; // 出力用の構造化バッファー

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pBufInputSRV = nullptr;        // 入力用の構造化バッファーから作成されるシェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	pBufResultUAV = nullptr;        // 出力用の構造化バッファーから作成されるアンオーダード アクセス ビュー

	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // 入力用バッファーの配列を宣言

	// てすとしめ---------------------------------------------------------------------
public:
	bool Init();
	void Update();
	void Draw();

	void SetSceneConstantBuffer(const ID3D11Buffer* cbBuf);

	void CreateConstantBuffer(ID3D11Buffer** dstBuf, size_t size);

};