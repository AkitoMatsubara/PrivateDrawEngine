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

		player = make_unique<Player>();
		player->Initialize();

		SkinnedShader = std::make_unique<ShaderEx>();
		SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");
	}

	// Compute Shaderセッティング
	{
		// シーンコンスタントバッファの設定
		{

			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth = sizeof(cs_constants);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer[1].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		ComputeShader = make_unique<ShaderEx>();
		ComputeShader->Create(L"Shaders\\ComputeShader_cs");

		// 入力用バッファーに初期値を設定する
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			vBufInArray[i].i = i;
			vBufInArray[i].f = static_cast<float>(NUM_ELEMENTS - 1 - i);
		}

		// コンピュートシェーダーへの入力時に使用するSRVを作成する
		CreateSRVForStructuredBuffer(sizeof(BUFIN_TYPE), NUM_ELEMENTS, &vBufInArray[0], pBufInput.GetAddressOf(), pBufInputSRV.GetAddressOf());

		// コンピュートシェーダーからの出力時に使用するUAVを作成する
		CreateUAVForStructuredBuffer(sizeof(BUFOUT_TYPE), NUM_ELEMENTS, NULL, pBufResult.GetAddressOf(), pBufResultUAV.GetAddressOf());
		}

	return true;
}

void SceneTest_2::Update() {
	imguiUpdate();
	const float elapsed_time = FRAMEWORK->GetElapsedTime();
	// シーン切り替え
	if (GetAsyncKeyState('G') < 0) setScene(std::make_unique<SceneTitle>());

	player->Update();

	// カメラ操作
	static float cameraSpeed = 7.0f;
	if (GetKeyState(VK_RIGHT) < 0)  eyePos.x += cameraSpeed * elapsed_time;	// 右に
	if (GetKeyState(VK_LEFT) < 0)  eyePos.x -= cameraSpeed * elapsed_time;	// 左に
	if (GetKeyState(VK_UP) < 0)  eyePos.z += cameraSpeed * elapsed_time;	// 前に
	if (GetKeyState(VK_DOWN) < 0)  eyePos.z -= cameraSpeed * elapsed_time;	// 後に
	if (GetKeyState(VK_SPACE) < 0)  eyePos.y += cameraSpeed * elapsed_time;	// 上に
	if (GetKeyState(VK_SHIFT) < 0)  eyePos.y -= cameraSpeed * elapsed_time;	// 下に

	{
		//// コンピュートシェーダーを実行する
		//RunComputeShader(pD3DDeviceContext, pComputeShader, pBufSRV, pBufResultUAV, NUM_ELEMENTS / 2, 1, 1);
		ComPtr<ID3D11DeviceContext> immediate_context= FRAMEWORK->GetDeviceContext();
		ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
		HRESULT hr = { S_OK };

		immediate_context->CSSetShader(ComputeShader->GetCS(), NULL, 0);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static float theta = 0;
		theta = (theta <= 1.0f) ? theta + 0.001 : 0.0f;
		//D3D11_MAPPED_SUBRESOURCE subRes;
		//immediate_context->Map(pBufInput.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
		//BUFIN_TYPE* pBufType = (BUFIN_TYPE*)subRes.pData;
		//pBufType->f += 5;
		////memcpy(subRes.pData, vBufInArray, sizeof(BUFIN_TYPE) * NUM_ELEMENTS);
		//immediate_context->Unmap(pBufInput.Get(), 0);

		// コンスタントバッファ更新
		cs_constants csData{};
		csData.Theta = theta;
		immediate_context->UpdateSubresource(constant_buffer[1].Get(), 0, 0, &csData, 0, 0);
		immediate_context->CSSetConstantBuffers(2, 1, constant_buffer[1].GetAddressOf());
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


		// SRVをCSに設定
		immediate_context->CSSetShaderResources(0, 1, pBufInputSRV.GetAddressOf());
		// UAVをCSに設定
		immediate_context->CSSetUnorderedAccessViews(0, 1, pBufResultUAV.GetAddressOf(),NULL);
		// CSの実行
		immediate_context->Dispatch(/*NUM_ELEMENTS / 2*/3, 1, 1);
		// CSの設定解除
		immediate_context->CSSetShader(NULL, NULL, 0);

		ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
		immediate_context->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

		ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
		immediate_context->CSSetShaderResources(0, 2, ppSRVNULL);

		ID3D11Buffer* ppCBNULL[1] = { NULL };
		immediate_context->CSSetConstantBuffers(0, 1, ppCBNULL);


		// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
		ID3D11Buffer* debugbuf = CreateAndCopyToBuffer(device.Get(), immediate_context.Get(), pBufResult.Get());

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		hr = immediate_context->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);	// 読み取り専用でマップ
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		BUFOUT_TYPE* p;
		ZeroMemory(&p, sizeof(BUFOUT_TYPE));
		p = reinterpret_cast<BUFOUT_TYPE*>(MappedResource.pData);	// 型変換して代入
		immediate_context->Unmap(debugbuf, 0);	// マップ解除

		// コンピュートシェーダーの結果を出力
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			printf("%d + %d = %d\n", i, NUM_ELEMENTS - 1 - i, p[i]);
		}

		skinned_mesh->setColor(p[0].i,p[1].i,p[2].i,1.0f);

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
			//skinned_mesh->Render(SkinnedShader.get());
			player->Render();
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

// アンオーダードアクセスビューのバッファの内容をCPUから読み込み可能なバッファを作成してコピーする
ID3D11Buffer* SceneTest_2::CreateAndCopyToBuffer(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, ID3D11Buffer* pBuffer)
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