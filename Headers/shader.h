#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <wrl.h>
using namespace DirectX;
using namespace Microsoft::WRL;

// 基底クラス
class Shader {
	// 関数
public:
protected:
	ComPtr<ID3D11VertexShader>	 VS = nullptr; // 頂点シェーダ
	ComPtr<ID3D11PixelShader>	 PS = nullptr; // ピクセルシェーダ
	ComPtr<ID3D11GeometryShader> GS = nullptr; // ジオメトリシェーダ
	ComPtr<ID3D11HullShader>	 HS = nullptr; // ハルシェーダ
	ComPtr<ID3D11DomainShader>	 DS = nullptr; // ドメインネームシェーダ
	ComPtr<ID3D11ComputeShader>	 CS = nullptr; // コンピュートシェーダ

	ComPtr<ID3D11InputLayout> InputLayout = nullptr;	// VS用
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
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;	// TextuerCoordinateの略、UV座標の取得などによく使われる様子
	XMFLOAT4 color;
};
