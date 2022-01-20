////-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#include <d3d11.h>
//#include <stdio.h>
//#include <tchar.h>
//#include <d3dcompiler.h>
//
//#define SAFE_RELEASE(x) if( x != NULL ){ x->Release(); x = NULL; }
//
//const UINT NUM_ELEMENTS = 128;
//
//struct BUFIN_TYPE
//{
//	int i;
//	float f;
//};
//
//struct BUFOUT_TYPE
//{
//	int i;
//};
//
//// 初期化
//HRESULT CreateComputeDevice(ID3D11Device** ppD3DDevice, ID3D11DeviceContext** ppD3DDeviceContext, ID3D11ComputeShader** ppComputeShader)
//{
//	HRESULT hr = E_FAIL;
//
//	// コンピュートシェーダーは SM 4 でも動作するらしいがサンプルでは対応しない
//	D3D_FEATURE_LEVEL FeatureLevel[] = {
//										   D3D_FEATURE_LEVEL_11_0,  // Direct3D 11.0 SM 5
//	};
//	UINT FeatureLevelLength = _countof(FeatureLevel);
//
//#if defined(DEBUG) || defined(_DEBUG)
//	// デバッグコンパイルの場合、デバッグレイヤーを有効にする。
//	UINT createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
//#else
//	UINT createDeviceFlag = 0;
//#endif
//
//	// デバイスを作成する。
//	hr = D3D11CreateDevice(
//		NULL,                       // 使用するアダプターを設定。NULLの場合はデフォルトのアダプター。
//		D3D_DRIVER_TYPE_HARDWARE,   // D3D_DRIVER_TYPEのいずれか。ドライバーの種類。
//		NULL,                       // ソフトウェアラスタライザを実装するDLLへのハンドル。
//		createDeviceFlag,           // D3D11_CREATE_DEVICE_FLAGの組み合わせ。デバイスを作成時に使用されるパラメータ。
//		FeatureLevel,               // D3D_FEATURE_LEVELのポインタ
//		FeatureLevelLength,         // D3D_FEATURE_LEVEL配列の要素数。
//		D3D11_SDK_VERSION,          // DirectX SDKのバージョン。この値は固定。
//		ppD3DDevice,                // 初期化されたデバイス
//		NULL,                       // 採用されたフィーチャーレベル。使用しないので NULL
//		ppD3DDeviceContext          // 初期化されたデバイスコンテキスト
//	);
//	if (FAILED(hr))
//		goto EXIT;
//
//	ID3D10Blob* pBlob = NULL;
//
//	UINT Flag1 = D3D10_SHADER_ENABLE_STRICTNESS;
//#if defined(DEBUG) || defined(_DEBUG)
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
//#else
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
//#endif
//
//	// ファイルを元にエフェクトをコンパイルする
//	hr = D3DCompileFromFile(_T("../../USER/HLSL/ComputeShader01.hlsl"), NULL, NULL, "CS_Main", "cs_5_0", Flag1, 0, &pBlob, NULL);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// コンパイル済みシェーダーから、コンピュートシェーダー オブジェクトを作成する
//	hr = (*ppD3DDevice)->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppComputeShader);
//	if (FAILED(hr))
//		goto EXIT;
//
//	hr = S_OK;
//
//EXIT:
//	//SAFE_RELEASE(pBlob);
//	return hr;
//}
//
//// コンピュートシェーダーへの入力時に使用するシェーダーリソースビューを作成する
//HRESULT CreateSRVForStructuredBuffer(ID3D11Device* pD3DDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
//	HRESULT hr = E_FAIL;
//
//	*ppBuf = NULL;
//	*ppSRVOut = NULL;
//
//	// 構造化バッファーを作成する
//
//	D3D11_BUFFER_DESC BufferDesc;
//	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
//	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |		// アンオーダード アクセス リソースをバインドする
//		D3D11_BIND_SHADER_RESOURCE;								// バッファーをシェーダー ステージにバインドする
//	BufferDesc.ByteWidth = uElementSize * uCount;					// バッファサイズ
//	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// 構造化バッファーとしてリソースを作成する
//	BufferDesc.StructureByteStride = uElementSize;					// 構造化バッファーのサイズ (バイト単位)
//
//	// 初期値を設定
//	if (pInitData)
//	{
//		D3D11_SUBRESOURCE_DATA InitData;
//		InitData.pSysMem = pInitData;
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//	// 初期値なしで領域のみ確保する
//	else
//	{
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//
//	// 構造化バッファーからシェーダーリソースビューを作成する
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
//	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // 拡張されたバッファーであることを指定する
//	SRVDesc.BufferEx.FirstElement = 0;
//	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
//	SRVDesc.BufferEx.NumElements = uCount;                  // リソース内の要素の数
//
//	// 構造化バッファーをもとにシェーダーリソースビューを作成する
//	hr = pD3DDevice->CreateShaderResourceView(*ppBuf, &SRVDesc, ppSRVOut);
//	if (FAILED(hr))
//		goto EXIT;
//
//	hr = S_OK;
//EXIT:
//	return hr;
//}
//
//// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
//HRESULT CreateUAVForStructuredBuffer(ID3D11Device* pD3DDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
//	HRESULT hr = E_FAIL;
//
//	*ppBuf = NULL;
//	*ppUAVOut = NULL;
//
//	// 構造化バッファーを作成する
//
//	D3D11_BUFFER_DESC BufferDesc;
//	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
//	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |          // アンオーダード アクセス リソースをバインドする
//		D3D11_BIND_SHADER_RESOURCE;            // バッファーをシェーダー ステージにバインドする
//	BufferDesc.ByteWidth = uElementSize * uCount;                  // バッファサイズ
//	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;  // 構造化バッファーとしてリソースを作成する
//	BufferDesc.StructureByteStride = uElementSize;                 // 構造化バッファーのサイズ (バイト単位)
//
//	// 初期値を設定
//	if (pInitData)
//	{
//		D3D11_SUBRESOURCE_DATA InitData;
//		InitData.pSysMem = pInitData;
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//	// 初期値なしで領域のみ確保する
//	else
//	{
//		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
//		if (FAILED(hr))
//			goto EXIT;
//	}
//
//	// 構造化バッファーからアンオーダードアクセスビューを作成する
//
//	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
//	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
//	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // バッファーであることを指定する
//	UAVDesc.Buffer.FirstElement = 0;
//	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
//	UAVDesc.Buffer.NumElements = uCount;                  // リソース内の要素の数
//
//	// 構造化バッファーをもとにアンオーダード アクセス ビューを作成する
//	hr = pD3DDevice->CreateUnorderedAccessView(*ppBuf, &UAVDesc, ppUAVOut);
//	if (FAILED(hr))
//		goto EXIT;
//
//	hr = S_OK;
//EXIT:
//	return hr;
//}
//
//// コンピュートシェーダーを実行する
//void RunComputeShader(ID3D11DeviceContext* pD3DDeviceContext, ID3D11ComputeShader* pComputeShader
//	, ID3D11ShaderResourceView* pBufSRV         // 入力用
//	, ID3D11UnorderedAccessView* pBufResultUAV  // 出力用
//	, UINT X, UINT Y, UINT Z) {
//	pD3DDeviceContext->CSSetShader(pComputeShader, NULL, 0);
//
//	// シェーダーリソースビューをコンピュートシェーダーに設定
//	pD3DDeviceContext->CSSetShaderResources(0, 1, &pBufSRV);
//
//	// アンオーダードアクセスビューをコンピュートシェーダーに設定
//	pD3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &pBufResultUAV, NULL);
//
//	// コンピュートシェーダーを実行する。いまいちわかってない
//	pD3DDeviceContext->Dispatch(X, Y, Z);
//
//	// コンピュートシェーダの設定解除
//	pD3DDeviceContext->CSSetShader(NULL, NULL, 0);
//
//	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
//	pD3DDeviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);
//
//	ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
//	pD3DDeviceContext->CSSetShaderResources(0, 2, ppSRVNULL);
//
//	ID3D11Buffer* ppCBNULL[1] = { NULL };
//	pD3DDeviceContext->CSSetConstantBuffers(0, 1, ppCBNULL);
//}
//
//// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
//ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer)
//{
//	ID3D11Buffer* debugbuf = NULL;
//
//	D3D11_BUFFER_DESC BufferDesc;
//	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
//	pBuffer->GetDesc(&BufferDesc);
//	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // CPU から読み込みできるように設定する
//	BufferDesc.Usage = D3D11_USAGE_STAGING;             // GPU から CPU へのデータ転送 (コピー) をサポートするリソース
//	BufferDesc.BindFlags = 0;
//	BufferDesc.MiscFlags = 0;
//	if (FAILED(pD3DDevice->CreateBuffer(&BufferDesc, NULL, &debugbuf)))
//		goto EXIT;
//
//	pD3DDeviceContext->CopyResource(debugbuf, pBuffer);
//
//EXIT:
//	return debugbuf;
//}
//
//int __cdecl mainTest() {
//	// 変数用意
//	HRESULT hr = E_FAIL;
//
//	ID3D11Device* pD3DDevice = NULL;					// Direct3D11 デバイス
//	ID3D11DeviceContext* pD3DDeviceContext = NULL;      // Direct3D11 デバイスコンテキスト
//	ID3D11ComputeShader* pComputeShader = NULL;         // コンピュートシェーダー インターフェース
//
//	ID3D11Buffer* pBuf = NULL;                          // 入力用の構造化バッファー
//	ID3D11Buffer* pBufResult = NULL;                    // 出力用の構造化バッファー
//
//	ID3D11ShaderResourceView* pBufSRV = NULL;           // 入力用の構造化バッファーから作成されるシェーダーリソースビュー
//	ID3D11UnorderedAccessView* pBufResultUAV = NULL;    // 出力用の構造化バッファーから作成されるアンオーダード アクセス ビュー
//
//	BUFIN_TYPE vBufInArray[NUM_ELEMENTS];               // 入力用バッファーの配列を宣言
//
//	// --Initialize--
//
//	// Direct3Dデバイス、およびコンピュートシェーダーを作成する
//	hr = CreateComputeDevice(&pD3DDevice, &pD3DDeviceContext, &pComputeShader);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// 入力用バッファーに初期値を設定する
//	for (int i = 0; i < NUM_ELEMENTS; i++)
//	{
//		vBufInArray[i].i = i;
//		vBufInArray[i].f = static_cast<float>(NUM_ELEMENTS - 1 - i);
//	}
//
//	// コンピュートシェーダーへの入力時に使用するシェーダーリソースビューを作成する
//	hr = CreateSRVForStructuredBuffer(pD3DDevice, sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], &pBuf, &pBufSRV);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
//	hr = CreateUAVForStructuredBuffer(pD3DDevice, sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, &pBufResult, &pBufResultUAV);
//	if (FAILED(hr))
//		goto EXIT;
//
//	// --Update--
//
//	// コンピュートシェーダーを実行する
//	RunComputeShader(pD3DDeviceContext, pComputeShader, pBufSRV, pBufResultUAV, NUM_ELEMENTS / 2, 1, 1);
//
//	// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
//	ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(pD3DDevice, pD3DDeviceContext, pBufResult);
//	D3D11_MAPPED_SUBRESOURCE MappedResource;
//	pD3DDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);
//	BUFOUT_TYPE* p = reinterpret_cast<BUFOUT_TYPE*>(MappedResource.pData);
//
//	//// コンピュートシェーダーの結果を出力
//	//for (int i = 0; i < NUM_ELEMENTS; i++)
//	//{
//	//	printf("%d + %d = %d\n", i, NUM_ELEMENTS - 1 - i, p[i]);
//	//}
//
//	pD3DDeviceContext->Unmap(debugbuf, 0);
//	SAFE_RELEASE(debugbuf);
//
//	char str[6];
//	scanf_s("%5s", str, 6);
//
//	hr = S_OK;
//
//EXIT:
//	SAFE_RELEASE(pBufSRV);
//	SAFE_RELEASE(pBufResultUAV);
//	SAFE_RELEASE(pBufResult);
//	SAFE_RELEASE(pBuf);
//
//	SAFE_RELEASE(pComputeShader);
//	SAFE_RELEASE(pD3DDeviceContext);
//	SAFE_RELEASE(pD3DDevice);
//
//	return 0;
//}