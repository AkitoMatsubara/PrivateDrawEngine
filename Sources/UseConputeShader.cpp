#include "UseConputeShader.h"
#include "framework.h"


// 構造化バッファを作成する
HRESULT UseConputeShader::CreateStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf)
{
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;

	// 構造化バッファーを作成する
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |			// アンオーダード アクセス リソースをバインドする
		D3D11_BIND_SHADER_RESOURCE;								// バッファーをシェーダー ステージにバインドする
	BufferDesc.ByteWidth = uElementSize * uCount;					// バッファサイズ
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// 構造化バッファーとしてリソースを作成する
	BufferDesc.StructureByteStride = uElementSize;					// 構造化バッファーのサイズ (バイト単位)

	// 初期値を設定
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
	}
	// 初期値なしで領域のみ確保する
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
	}
	return hr;
}

// コンピュートシェーダーへの入力時に使用するシェーダーリソースビューを作成する
HRESULT UseConputeShader::CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppSRVOut = NULL;

	// 構造化バッファーを作成する
	hr = UseConputeShader::CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	// 構造化バッファーからシェーダーリソースビューを作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // 拡張されたバッファーであることを指定する
	SRVDesc.BufferEx.FirstElement = 0;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.BufferEx.NumElements = uCount;                  // リソース内の要素の数

	// 構造化バッファーをもとにシェーダーリソースビューを作成する
	hr = pD3DDevice->CreateShaderResourceView(*ppBuf, &SRVDesc, ppSRVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateShaderResourceView");

	return hr;
}

// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
HRESULT UseConputeShader::CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppUAVOut = NULL;

	// 構造化バッファーを作成する
	hr = UseConputeShader::CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	// 構造化バッファーからアンオーダードアクセスビューを作成する
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // バッファーであることを指定する
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = uCount;                  // リソース内の要素の数

	// 構造化バッファーをもとにアンオーダード アクセス ビューを作成する
	hr = pD3DDevice->CreateUnorderedAccessView(*ppBuf, &UAVDesc, ppUAVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateUnorderedAccessView");

	return hr;
}

// アンオーダードアクセスビューのバッファの内容をCPUから読み込み可能なバッファを作成してコピーする
ID3D11Buffer* UseConputeShader::CreateAndCopyToBuffer(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	pBuffer->GetDesc(&BufferDesc);
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // CPU から読み込みできるように設定する
	BufferDesc.Usage = D3D11_USAGE_STAGING;             // GPU から CPU へのデータ転送 (コピー) をサポートするリソース
	BufferDesc.BindFlags = 0;
	BufferDesc.MiscFlags = 0;
	if (FAILED(pD3DDevice->CreateBuffer(&BufferDesc, NULL, &debugbuf))) return debugbuf;

	pD3DDeviceContext->CopyResource(debugbuf, pBuffer);

	return debugbuf;
}

void UseConputeShader::RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV, ID3D11UnorderedAccessView* pDstUAV, UINT X, UINT Y, UINT Z) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	immediate_context->CSSetShader(pComputeShader, NULL, 0);

	// シェーダーリソースビューをコンピュートシェーダーに設定
	immediate_context->CSSetShaderResources(0, 1, &pSrcSRV);

	// アンオーダードアクセスビューをコンピュートシェーダーに設定
	immediate_context->CSSetUnorderedAccessViews(0, 1, &pDstUAV, NULL);

	// コンピュートシェーダーを実行する。スレッド数とかようわからんが
	immediate_context->Dispatch(X, Y, Z);

	// コンピュートシェーダの設定解除
	immediate_context->CSSetShader(NULL, NULL, 0);

	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
	immediate_context->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

	ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
	immediate_context->CSSetShaderResources(0, 2, ppSRVNULL);

	ID3D11Buffer* ppCBNULL[1] = { NULL };
	immediate_context->CSSetConstantBuffers(0, 1, ppCBNULL);
}