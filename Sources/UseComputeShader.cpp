#include "UseComputeShader.h"
#include "framework.h"


// 構造化バッファを作成する
HRESULT UseComputeShader::CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf)
{
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;

	// 構造化バッファーを作成する
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags =	D3D11_BIND_UNORDERED_ACCESS |			// アンオーダード アクセス リソースをバインドする
							D3D11_BIND_SHADER_RESOURCE;				// バッファーをシェーダー ステージにバインドする
	BufferDesc.ByteWidth = uElementSize * uCount;					// バッファサイズ
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// 構造化バッファーとしてリソースを作成する
	BufferDesc.StructureByteStride = uElementSize;					// 構造化バッファーのサイズ (バイト単位)

	// 初期値を設定
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
	}
	// 初期値なしで領域のみ確保する
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, nullptr, ppBuf);
	}
	if (FAILED(hr))_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	return hr;
}

// CSからの入力時に使用するSRVと構造化バッファ作成する
HRESULT UseComputeShader::CreateStructuredBufferAndSRV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = nullptr;
	*ppSRVOut = nullptr;

	// 構造化バッファーを作成する
	hr = UseComputeShader::CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	// 構造化バッファーからシェーダーリソースビューを作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // SRVであることを指定する
	SRVDesc.BufferEx.FirstElement = 0;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.BufferEx.NumElements = uCount;                  // リソース内の要素の数

	// 構造化バッファーをもとにシェーダーリソースビューを作成する
	hr = pD3DDevice->CreateShaderResourceView(*ppBuf, &SRVDesc, ppSRVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateShaderResourceView");

	return hr;
}

// CSからの入力時に使用するSRVを構造化バッファから作成する
HRESULT UseComputeShader::CreateSRVfromStructuredBuffer(ID3D11Buffer* ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppSRVOut = nullptr;

	D3D11_BUFFER_DESC BufDesc;
	ZeroMemory(&BufDesc, sizeof(D3D11_BUFFER_DESC));
	ppBuf->GetDesc(&BufDesc);	// サイズの計算のために構造化バッファのDescを取得

	// 構造化バッファーからシェーダーリソースビューを作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // SRVであることを指定する
	SRVDesc.BufferEx.FirstElement = 0;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.BufferEx.NumElements = BufDesc.ByteWidth / BufDesc.StructureByteStride; // リソース内の要素の数  総数から構造体のサイズを割れば個数が手に入る

	// 構造化バッファーをもとにシェーダーリソースビューを作成する
	hr = pD3DDevice->CreateShaderResourceView(ppBuf, &SRVDesc, ppSRVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateShaderResourceView");

	return hr;
}

// CSからの出力時に使用するUAVと構造化バッファ作成する
HRESULT UseComputeShader::CreateStructuredBufferAndUAV(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = nullptr;
	*ppUAVOut = nullptr;

	// 構造化バッファーを作成する
	hr = UseComputeShader::CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateStructuredBuffer");

	// 構造化バッファーからアンオーダードアクセスビューを作成する
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // UAVであることを指定する
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = uCount;                  // リソース内の要素の数

	// 構造化バッファーをもとにアンオーダード アクセス ビューを作成する
	hr = pD3DDevice->CreateUnorderedAccessView(*ppBuf, &UAVDesc, ppUAVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateUnorderedAccessView");

	return hr;
}

// CSからの出力時に使用するUAVと構造化バッファ作成する
HRESULT UseComputeShader::CreateUAVfromStructuredBuffer(ID3D11Buffer* ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppUAVOut = nullptr;

	D3D11_BUFFER_DESC BufDesc;
	ZeroMemory(&BufDesc, sizeof(D3D11_BUFFER_DESC));
	ppBuf->GetDesc(&BufDesc);	// サイズの計算のために構造化バッファのDescを取得

	// 構造化バッファーからアンオーダードアクセスビューを作成する
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // UAVであることを指定する
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = BufDesc.ByteWidth / BufDesc.StructureByteStride; // リソース内の要素の数  総数から構造体のサイズを割れば個数が手に入る

	// 構造化バッファーをもとにアンオーダード アクセス ビューを作成する
	hr = pD3DDevice->CreateUnorderedAccessView(ppBuf, &UAVDesc, ppUAVOut);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateUnorderedAccessView");

	return hr;
}

// アンオーダードアクセスビューのバッファ(GPU)の内容をCPUから読み込み可能なバッファを作成してコピーする
void UseComputeShader::CreateAndCopyToBuffer(ID3D11Buffer* pSrcBuf,ID3D11Buffer** ppDstBuf)
{
	ID3D11Device* device = FRAMEWORK->GetDevice();
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();
	HRESULT hr = E_FAIL;

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	pSrcBuf->GetDesc(&BufferDesc);	// 引数のBUFFER_DESCを取得
	// 中身をCPUで読み込むめるようににDESCを再設定
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // CPU から読み込みできるように設定する
	BufferDesc.Usage = D3D11_USAGE_STAGING;             // GPU から CPU へのデータ転送 (コピー) をサポートするリソース CPU読み書き専用
	BufferDesc.BindFlags = 0;
	BufferDesc.MiscFlags = 0;

	// 再設定したDESCでbufferを作成
	hr = device->CreateBuffer(&BufferDesc, nullptr, ppDstBuf);
	if (FAILED(hr))	_ASSERT_EXPR_A(false, "FAILED CreateReadBackBuffer. Not Set Shader?");

	// 引数bufferのデータを作成したbufferにコピー
	immediate_context->CopyResource(*ppDstBuf, pSrcBuf);

}

void UseComputeShader::RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV, UINT SRVslot, ID3D11UnorderedAccessView* pDstUAV, UINT UAVslot, UINT X, UINT Y, UINT Z) {
	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();

	// 使うCSを設定
	immediate_context->CSSetShader(pComputeShader, nullptr, 0);

	// シェーダーリソースビューをコンピュートシェーダーに設定
	immediate_context->CSSetShaderResources(SRVslot, 1, &pSrcSRV);

	// アンオーダードアクセスビューをコンピュートシェーダーに設定
	immediate_context->CSSetUnorderedAccessViews(UAVslot, 1, &pDstUAV, nullptr);

	// コンピュートシェーダーを実行する。スレッド数とかようわからんがこれ
	immediate_context->Dispatch(X, Y, Z);

	// コンピュートシェーダの設定解除
	immediate_context->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { nullptr };
	immediate_context->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, nullptr);

	ID3D11ShaderResourceView* ppSRVNULL[2] = { nullptr, nullptr };
	immediate_context->CSSetShaderResources(0, 2, ppSRVNULL);

	ID3D11Buffer* ppCBNULL[1] = { nullptr };
	immediate_context->CSSetConstantBuffers(0, 1, ppCBNULL);
}