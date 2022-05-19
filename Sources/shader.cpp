#include "framework.h"
#include "shader.h"

using namespace Microsoft::WRL;

HRESULT Shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut) {
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	ComPtr<ID3DBlob> BlobError = NULL;
	// コンパイル
	// ファイル名,2,3,シェーダのエントリポイント,シェーダモデル,6,7,結果を受け取るID3DpBolb*,9 必須のみ記載
	HRESULT hr = D3DCompileFromFile(filename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, method, shaderModel, ShaderFlags, 0, ppBlobOut, BlobError.GetAddressOf());

	// エラー出力
	if (BlobError != NULL)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
	}

	return hr;
}

bool Shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> VSBlob = NULL;	// VSのコンパイル結果を受け取るBlob
	// 頂点シェーダ
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 頂点シェーダ生成
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// 入力レイアウト生成
	hr = device->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), InputLayout.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ピクセルシェーダ
	ComPtr<ID3DBlob> PSBlob = NULL;	// PSのコンパイル結果を受け取るBlob
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr)){
		return false;
	}

	// ピクセルシェーダ生成
	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

void Shader::Activate(ID3D11DeviceContext* device_context) {
	// 0609 入力レイアウト設定
	device_context->IASetInputLayout(InputLayout.Get());

	device_context->VSSetShader(VS.Get(), NULL, 0);
	device_context->PSSetShader(PS.Get(), NULL, 0);
	//device_context->HSSetShader(HS.Get(), NULL, 0);
	//device_context->DSSetShader(DS.Get(), NULL, 0);
	device_context->GSSetShader(GS.Get(), NULL, 0);
}

void Shader::Inactivate(ID3D11DeviceContext* device_context) {

	// 0609 入力レイアウト設定
	device_context->IASetInputLayout(InputLayout.Get());

	device_context->VSSetShader(NULL, NULL, 0);
	device_context->PSSetShader(NULL, NULL, 0);
	//device_context->HSSetShader(NULL, NULL, 0);
	//device_context->DSSetShader(NULL, NULL, 0);
	device_context->GSSetShader(NULL, NULL, 0);
}
