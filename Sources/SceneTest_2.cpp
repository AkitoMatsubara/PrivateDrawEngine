#include "SceneTest_2.h"
#include "SceneTitle.h"
#include <SimpleMath.h>

bool SceneTest_2::Initialize() {
	Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();	// frameworkからdeviceを取得
	HRESULT hr = { S_OK };

	// シーンコンスタントバッファの設定
	CreateConstantBuffer(constant_buffer[0].GetAddressOf(), sizeof(scene_constants));

	// Samplerの設定
	sampleClamp = std::make_shared<Sampler>(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	// 各種クラス設定
	{
		camera = std::make_unique<Camera>();

		// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
		sprites = std::make_unique<Sprite>(L".\\Resources\\screenshot.jpg");	// シェーダーはコンストラクタ内で指定しているため、別を使うには改良が必要
		sprites->setSize(1280, 720);
		SpriteShader = std::make_unique<ShaderEx>();
		SpriteShader->Create(L"Shaders\\sprite_vs", L"Shaders\\sprite_ps");

		// Geometric_primitiveオブジェクトの生成
		{
			grid = std::make_unique<Geometric_Cube>();
			grid->setPos(DirectX::SimpleMath::Vector3(0, -1, 0));
			grid->setSize(DirectX::SimpleMath::Vector3(10, 0.1f, 10));
		}

		player = std::make_unique<Player>();
		player->Initialize();

		EnemyManager::getInstance().Initialize();

		stage = std::make_unique<Stage>();
		stage->Initialize();

		SkinnedShader = std::make_unique<ShaderEx>();
		SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");
	}

	// Compute Shaderセッティング
	{
		// CS用コンスタントバッファの設定
		CreateConstantBuffer(constant_buffer[1].GetAddressOf(), sizeof(cs_constants));

		ComputeShader = std::make_unique<ShaderEx>();
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
	if (GetAsyncKeyState('G') & 1) setScene(std::make_unique<SceneTitle>());

	player->Update();
	stage->Update();

	// カメラ操作
	camera->Operate();

	// 透視投影行列の作成
	camera->Set((camera->GetPos() + player->Parameters->Position), player->Parameters->Position, DirectX::XMFLOAT3(0, 1, 0));
	camera->SetProjection(DirectX::XMConvertToRadians(30), camera->GetWidth() / camera->GetHeight(), camera->GetNear(), camera->GetFar());

	if (GetAsyncKeyState(VK_RBUTTON) &1) {
		EnemyManager::getInstance().newSet(player->Parameters.get());	// お試し右クリックで敵を生成
	}
	EnemyManager::getInstance().Update();

	{
		// コンピュートシェーダーを実行する
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context = FRAMEWORK->GetDeviceContext();
		Microsoft::WRL::ComPtr<ID3D11Device> device = FRAMEWORK->GetDevice();
		HRESULT hr = { S_OK };

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static float theta = 0.0f;
		theta = (theta <= 1.0f) ? theta + 0.01f : 0.0f;	// チカチカすりゅ～！(色が)
		//D3D11_MAPPED_SUBRESOURCE subRes;	// 別の更新方法 のはず。未完成
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
		RunComputeShader(ComputeShader->GetCS(), pBufInputSRV.Get(), pBufResultUAV.Get(), 3, 1, 1);

		// アンオーダードアクセスビューのバッファの内容を CPU から読み込み可能なバッファへコピーする
		ID3D11Buffer* debugbuf = CreateAndCopyToBuffer(device.Get(), immediate_context.Get(), pBufResult.Get());

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		hr = immediate_context->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);	// 読み取り専用でマップ
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		BUFOUT_TYPE* p;
		ZeroMemory(&p, sizeof(BUFOUT_TYPE));
		p = reinterpret_cast<BUFOUT_TYPE*>(MappedResource.pData);	// 型変換して代入
		immediate_context->Unmap(debugbuf, 0);	// マップ解除
		debugbuf->Release();	// CS受け取りポインタを解放

		player->Parameters->Color = DirectX::SimpleMath::Vector4{ p[1].i, p[0].i, p[2].i, 1.0f };
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
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE), 1);		// 3Dオブジェクトの後ろに出すため一旦
		sprites->Render(SpriteShader.get());
		immediate_context->OMSetDepthStencilState(FRAMEWORK->GetDepthStencileState(DS_TRUE_WRITE), 1);	// 2Dオブジェクトとの前後関係をしっかりするため再設定
	}
	// 3Dオブジェクトの描画設定
	{
		camera->Activate();

		// コンスタントバッファ更新
		scene_constants data{};
		XMStoreFloat4x4(&data.view_projection, camera->GetView() * camera->GetProjection());	// Matrixから4x4へ変換
		data.light_direction = DirectX::SimpleMath::Vector4{ light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = DirectX::SimpleMath::Vector4{ camera->GetPos().x,camera->GetPos().y,camera->GetPos().z,0 };				// シェーダに渡すカメラの位置
		immediate_context->UpdateSubresource(constant_buffer[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());

		{
			grid->Render(true);
			stage->Render();
			player->Render();
			EnemyManager::getInstance().Render();
		}
	}

#ifdef USE_IMGUI
	//	ImGui::Render();
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else
	FRAMEWORK->Flip();
#endif
}

void SceneTest_2::imguiUpdate() {
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.6f, 0.1f, 1.0f));	// これ一つ呼ぶとImGui::PopStyleColorを書かないといけないらしい
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));

	imguiSceneChanger();
	// 2D用 内部関数で完結させてる
	sprites->ImguiWindow();
	// 3D用パラメータ
	player->ImguiPlayer();
	// ライト調整等グローバル設定
	ImGui::Begin("Light");
	ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
	ImGui::Checkbox("focus Zero", &focus_zero);
	ImGui::Text("Enemys: %d", EnemyManager::getInstance().getEnemys()->size());
	ImGui::Text("Total Objects: %d", EnemyManager::getInstance().getEnemys()->size() + player->getShotManager()->getSize());

	ImGui::PopStyleColor(2);	// ImGui::PushStyleColor一つにつき一つ呼び出すっぽい

	ImGui::End();
#endif
}

// 構造化バッファを作成する
HRESULT SceneTest_2::CreateStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf)
{
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;

	// 構造化バッファーを作成する

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |			// アンオーダード アクセス リソースをバインドする
		D3D11_BIND_SHADER_RESOURCE;				// バッファーをシェーダー ステージにバインドする
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
HRESULT SceneTest_2::CreateSRVForStructuredBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBuf, ID3D11ShaderResourceView** ppSRVOut) {
	ID3D11Device* pD3DDevice = FRAMEWORK->GetDevice();
	HRESULT hr = E_FAIL;
	*ppBuf = NULL;
	*ppSRVOut = NULL;

	// 構造化バッファーを作成する
	hr = CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))
	{
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

	hr = CreateStructuredBuffer(uElementSize, uCount, pInitData, ppBuf);
	if (FAILED(hr))
	{
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

void SceneTest_2::RunComputeShader(ID3D11ComputeShader* pComputeShader, ID3D11ShaderResourceView* pSrcSRV, ID3D11UnorderedAccessView* pDstUAV, UINT X, UINT Y, UINT Z) {
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