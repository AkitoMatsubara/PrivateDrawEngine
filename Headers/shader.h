#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <d3dcompiler.h>

#include <wrl.h>

// 基底クラス
class Shader {
	// 関数
public:
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	 VS = nullptr; // 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	 PS = nullptr; // ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> GS = nullptr; // ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11HullShader>	 HS = nullptr; // ハルシェーダ
	Microsoft::WRL::ComPtr<ID3D11DomainShader>	 DS = nullptr; // ドメインネームシェーダ
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>	 CS = nullptr; // コンピュートシェーダ

	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout = nullptr;	// VS用
private:
	// 変数
public:
	Shader() {};
	virtual ~Shader() {};

	// VS,PSの作成
	bool Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);

	void Activate();
	void Inactivate();

protected:
	// ファイルパス, エントリポイント, シェーダモデル, 受け取りID3DBlobポインタ
	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
private:
};

// 頂点フォーマット
struct Vertex {
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::XMFLOAT2 texcoord;	// TextuerCoordinateの略、UV座標の取得などによく使われる様子
	DirectX::SimpleMath::Vector4 color;
};
