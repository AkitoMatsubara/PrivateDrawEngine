#include "SceneTest_2.h"
#include "SceneTitle.h"

bool SceneTest_2::Initialize() {
	ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// frameworkからdeviceを取得
	HRESULT hr = { S_OK };

	// シーンコンスタントバッファの設定
	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(scene_constants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// Samplerの設定
	sampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// 各種クラス設定
	{
		// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
		sprites = make_unique<Sprite>(L".\\Resources\\screenshot.jpg");	// シェーダーはコンストラクタ内で指定しているため、別を使うには改良が必要
		sprites->setSize(1280, 720);
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->Create(L"Shaders\\sprite_vs", L"Shaders\\sprite_ps");

		// Geometric_primitiveオブジェクトの生成
		{
			grid = make_unique<Geometric_Cube>();
			grid->setPos(XMFLOAT3(0, -1, 0));
			grid->setSize(XMFLOAT3(10, 0.1f, 10));
			GeomtricShader = std::make_unique<ShaderEx>();
			GeomtricShader->Create(L"Shaders\\geometric_primitive_vs", L"Shaders\\geometric_primitive_ps");
		}

		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.000.fbx");		// テクスチャ、マテリアル無し
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.001.0.fbx");	// テクスチャ使用
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.001.1.fbx");	// 埋め込みテクスチャ
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.002.0.fbx");	// 3種テクスチャ使用
		//skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\Resources\\cube.002.1.fbx");	// テクスチャ有り無し、マテリアル有り無し混合
		skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.0.fbx");	// 複数メッシュ キューブと猿
		//skinned_mesh = make_unique<Skinned_Mesh>(".\\Resources\\cube.003.1.fbx", Skinned_Mesh::CST_RIGHT_Z, true);	// 3角形化されていない複数メッシュ キューブ
		SkinnedShader = std::make_unique<ShaderEx>();
		SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");
	}

	// Compute Shaderセッティング
	{
		ComputeShader = make_unique<ShaderEx>();
		ComputeShader->Create(L"Shaders\\ComputeShader_cs");

		// 入力用バッファーに初期値を設定する
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].i = i;
			vBufInArray[i].f = static_cast<float>(NUM_ELEMENTS - 1 - i);
		}

		// コンピュートシェーダーへの入力時に使用するシェーダーリソースビューを作成する
		CreateSRVForStructuredBuffer(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());
		//{
		//	// 構造化バッファの作成
		//	D3D11_BUFFER_DESC BufDesc;
		//	ZeroMemory(&BufDesc, sizeof(D3D11_BUFFER_DESC));
		//	BufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;	// 2種でバインド
		//	BufDesc.ByteWidth = sizeof(BUFIN_TYPE) * NUM_ELEMENTS;		// バッファサイズ
		//	BufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// 構造化バッファとしてリソースを作成
		//	BufDesc.StructureByteStride = sizeof(BUFIN_TYPE);			// 構造化バッファのサイズ
		//	//初期値の設定
		//	if (&vBufInArray[0]) {
		//		D3D11_SUBRESOURCE_DATA InitData;
		//		InitData.pSysMem = &vBufInArray[0];
		//		hr = device->CreateBuffer(&BufDesc, &InitData, pBufInput.GetAddressOf());
		//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//	}
		//	// 初期値無しで領域のみ確保する
		//	else {
		//		hr = device->CreateBuffer(&BufDesc, NULL, pBufInput.GetAddressOf());
		//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//	}
		//	// 構造化バッファからSRVを作成する
		//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		//	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;	// 拡張されたバッファであることを指定する
		//	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;			// 形状不明フォーマットということ
		//	SRVDesc.BufferEx.FirstElement = 0;				// 最初にアクセスするインデックス番号
		//	SRVDesc.BufferEx.NumElements = NUM_ELEMENTS;	// リソース内の要素数
		//	// 構造化バッファをもとにSRVを作成する
		//	hr = device->CreateShaderResourceView(pBufInput.Get(), &SRVDesc, pBufInputSRV.GetAddressOf());
		//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//}


		// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
		CreateUAVForStructuredBuffer(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
		//{
		//	// 構造化バッファの作成
		//	D3D11_BUFFER_DESC BufDesc;
		//	ZeroMemory(&BufDesc, sizeof(D3D11_BUFFER_DESC));
		//	BufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;	// 2種でバインド
		//	BufDesc.ByteWidth = sizeof(BUFIN_TYPE) * NUM_ELEMENTS;		// バッファサイズ
		//	BufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// 構造化バッファとしてリソースを作成
		//	BufDesc.StructureByteStride = sizeof(BUFIN_TYPE);			// 構造化バッファのサイズ
		//	//初期値の設定
		//	if (&vBufInArray[0]) {
		//		D3D11_SUBRESOURCE_DATA InitData;
		//		InitData.pSysMem = &vBufInArray[0];
		//		hr = device->CreateBuffer(&BufDesc, &InitData, pBufResult.GetAddressOf());
		//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//	}
		//	// 初期値無しで領域のみ確保する
		//	else {
		//		hr = device->CreateBuffer(&BufDesc, NULL, pBufResult.GetAddressOf());
		//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//	}
		//	// 構造化バッファからSRVを作成する
		//	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		//	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		//	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;	// バッファであることを指定する
		//	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;			// 形状不明フォーマットということ
		//	UAVDesc.Buffer.FirstElement = 0;				// 最初にアクセスするインデックス番号
		//	UAVDesc.Buffer.NumElements = NUM_ELEMENTS;	// リソース内の要素数
		//	// 構造化バッファをもとにSRVを作成する
		//	hr = device->CreateUnorderedAccessView(pBufInput.Get(), &UAVDesc, pBufResultUAV.GetAddressOf());
		//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//}
		}

	return true;
}

void SceneTest_2::Update() {
	imguiUpdate();
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// シーン切り替え
	if (GetAsyncKeyState('G') &1) setScene(std::make_unique<SceneTitle>());


	// カメラ操作
	static float speed = 7.0f;
	if (GetKeyState('D') < 0)  eyePos.x += speed * elapsed_time;	// 右に
	if (GetKeyState('A') < 0)  eyePos.x -= speed * elapsed_time;	// 左に
	if (GetKeyState('W') < 0)  eyePos.z += speed * elapsed_time;	// 前に
	if (GetKeyState('S') < 0)  eyePos.z -= speed * elapsed_time;	// 後ろに
	if (GetKeyState(VK_SPACE) < 0)  eyePos.y += speed * elapsed_time;	// 上に
	if (GetKeyState(VK_SHIFT) < 0)  eyePos.y -= speed * elapsed_time;	// 下に

	{
		//// コンピュートシェーダーを実行する
		//RunComputeShader(pD3DDeviceContext, pComputeShader, pBufSRV, pBufResultUAV, NUM_ELEMENTS / 2, 1, 1);
		ComPtr<ID3D11DeviceContext> DevCon= FRAMEWORK->GetDeviceContext();
		ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
		HRESULT hr = { S_OK };

		DevCon->CSSetShader(ComputeShader->GetCS(), NULL, 0);
		//DevCon->CSSetShader(cs.Get(), NULL, 0);
		// SRVをCSに設定
		DevCon->CSSetShaderResources(0, 1, pBufInputSRV.GetAddressOf());
		// UAVをCSに設定
		DevCon->CSSetUnorderedAccessViews(0, 1, pBufResultUAV.GetAddressOf(),NULL);
		// CSの実行
		DevCon->Dispatch(/*NUM_ELEMENTS / 2*/1, 2, 1);
		// CSの設定解除
		DevCon->CSSetShader(NULL, NULL, 0);

		ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
		DevCon->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

		ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
		DevCon->CSSetShaderResources(0, 2, ppSRVNULL);

		ID3D11Buffer* ppCBNULL[1] = { NULL };
		DevCon->CSSetConstantBuffers(0, 1, ppCBNULL);


		// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(device.Get(), DevCon.Get(), pBufResult.Get());

		//ID3D11Buffer* debugbuf = NULL;
		//D3D11_BUFFER_DESC BufferDesc;
		//ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		//pBufResult->GetDesc(&BufferDesc);	// バッファの初期化情報を取得
		//BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;	// CPUから読み込み出来るように設定する
		//BufferDesc.Usage = D3D11_USAGE_STAGING;				// GPUからCPUへのデータ転送をサポート
		//BufferDesc.BindFlags = 0;
		//BufferDesc.MiscFlags = 0;
		//hr = device->CreateBuffer(&BufferDesc, NULL, &debugbuf);	// 再設定したDescからバッファを作成
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//DevCon->CopyResource(debugbuf, pBufResult.Get());	// バッファの中身をコピー

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		DevCon->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);	// 読み取り専用でマップ
		BUFOUT_TYPE* p;
		ZeroMemory(&p, sizeof(BUFOUT_TYPE));
		p = reinterpret_cast<BUFOUT_TYPE*>(MappedResource.pData);	// 型変換して代入

		// コンピュートシェーダーの結果を出力
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			printf("%d + %d = %d\n", i, NUM_ELEMENTS - 1 - i, p[i]);
		}

		DevCon->Unmap(debugbuf, 0);	// マップ解除
		skinned_mesh->setPos(p[0].i*0.1f, p[2].i, p[4].i);

		hr = S_OK;
	}
}

void SceneTest_2::Render() {
	HRESULT hr{ S_OK };

	ID3D11DeviceContext* immediate_context = FRAMEWORK->GetDeviceContext();	// DevCon取得

	FRAMEWORK->Clear(ClearColor);	// 一旦クリア

	// ビューポートの設定
	FRAMEWORK->CreateViewPort();

	// サンプラーステートをバインド
	sampleClamp->Set(0);

	immediate_context->OMSetBlendState(FRAMEWORK->GetBlendState(FRAMEWORK->BS_ALPHA), nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)

	// 2Dオブジェクトの描画設定
	{
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE), 1);	// 3Dオブジェクトの後ろに出すため一旦
		sprites->Render(SpriteShader.get());
	}
	// 3Dオブジェクトの描画設定
	{
		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		immediate_context->RSGetViewports(&num_viewports, &viewport);	// ラスタライザステージにバインドされたviewportの配列を取得

		float aspect_ratio{ viewport.Width / viewport.Height };	// アスペクト比
		// 透視投影行列の作成
		XMMATRIX P{ XMMatrixPerspectiveFovLH(XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };	// 視野角,縦横比,近くのZ,遠くのZ

		XMVECTOR eye{ XMVectorSet(eyePos.x,eyePos.y,eyePos.z,1.0f) };
		XMVECTOR focus;
		if (!focus_zero) {
			//focus = { XMVectorSet(eyePos.x,eyePos.y,eyePos.z + 1,1.0f) };	// カメラ位置の前
			focus = { XMVectorSet(skinned_mesh->getPos().x,skinned_mesh->getPos().y,skinned_mesh->getPos().z,1.0f) };	// カメラ位置の前
		}
		else {
			focus = { XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
		}
		XMVECTOR up{ XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
		// ViewMatrixの作成(LH = LeftHand(左手座標系))
		XMMATRIX V{ XMMatrixLookAtLH(eye, focus, up) };	// カメラ座標、焦点、カメラの上方向

		// コンスタントバッファ更新
		scene_constants data{};
		XMStoreFloat4x4(&data.view_projection, V * P);	// Matrixから4x4へ変換
		data.light_direction = { light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = { eyePos.x,eyePos.y,eyePos.z,0 };				// シェーダに渡すカメラの位置
		immediate_context->UpdateSubresource(constant_buffer[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());

		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設定

		{
			// 3DオブジェクトRender内に移植 現状ここである必要なし？
			grid->wireframe = true;
			grid->Render(GeomtricShader.get());
			skinned_mesh->Render(SkinnedShader.get());
		}
	}

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	FRAMEWORK->Flip();
}

// コンピュートシェーダーへの入力時に使用するシェーダーリソースビューを作成する
HRESULT SceneTest_2::CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppSRVOut = NULL;

	// 構造化バッファーを作成する

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |		// アンオーダード アクセス リソースをバインドする
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
		if (FAILED(hr))
			goto EXIT;
	}
	// 初期値なしで領域のみ確保する
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
		if (FAILED(hr))
			goto EXIT;
	}

	// 構造化バッファーからシェーダーリソースビューを作成する

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // 拡張されたバッファーであることを指定する
	SRVDesc.BufferEx.FirstElement = 0;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.BufferEx.NumElements = uCount;                  // リソース内の要素の数

	// 構造化バッファーをもとにシェーダーリソースビューを作成する
	hr = pD3DDevice->CreateShaderResourceView(*ppBuf, &SRVDesc, ppSRVOut);
	if (FAILED(hr))
		goto EXIT;

	hr = S_OK;
EXIT:
	return hr;
}

// コンピュートシェーダーからの出力時に使用するアンオーダードアクセスビューを作成する
HRESULT SceneTest_2::CreateUAVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppUAVOut = NULL;

	// 構造化バッファーを作成する

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |          // アンオーダード アクセス リソースをバインドする
		D3D11_BIND_SHADER_RESOURCE;            // バッファーをシェーダー ステージにバインドする
	BufferDesc.ByteWidth = uElementSize * uCount;                  // バッファサイズ
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;  // 構造化バッファーとしてリソースを作成する
	BufferDesc.StructureByteStride = uElementSize;                 // 構造化バッファーのサイズ (バイト単位)

	// 初期値を設定
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		hr = pD3DDevice->CreateBuffer(&BufferDesc, &InitData, ppBuf);
		if (FAILED(hr))
			goto EXIT;
	}
	// 初期値なしで領域のみ確保する
	else
	{
		hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, ppBuf);
		if (FAILED(hr))
			goto EXIT;
	}

	// 構造化バッファーからアンオーダードアクセスビューを作成する

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;   // バッファーであることを指定する
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.NumElements = uCount;                  // リソース内の要素の数

	// 構造化バッファーをもとにアンオーダード アクセス ビューを作成する
	hr = pD3DDevice->CreateUnorderedAccessView(*ppBuf, &UAVDesc, ppUAVOut);
	if (FAILED(hr))
		goto EXIT;

	hr = S_OK;
EXIT:
	return hr;
}

// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
ID3D11Buffer* SceneTest_2::CreateAndCopyToDebugBuf(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	pBuffer->GetDesc(&BufferDesc);
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // CPU から読み込みできるように設定する
	BufferDesc.Usage = D3D11_USAGE_STAGING;             // GPU から CPU へのデータ転送 (コピー) をサポートするリソース
	BufferDesc.BindFlags = 0;
	BufferDesc.MiscFlags = 0;
	if (FAILED(pD3DDevice->CreateBuffer(&BufferDesc, NULL, &debugbuf)))
		goto EXIT;

	pD3DDeviceContext->CopyResource(debugbuf, pBuffer);

EXIT:
	return debugbuf;
}

void SceneTest_2::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	imguiSceneChanger();
	// 2D用 内部関数で完結させてる
	sprites->ImguiWindow();
	// 3D用パラメータ
	skinned_mesh->imguiWindow("fbx");

	// ライト調整等グローバル設定
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::End();
#endif
}