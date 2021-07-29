#include "framework.h"

//#define COMPARISON	// SpriteとSprite_Batchの描画速度比較
//static bool spriteBatch = false;

framework* framework::instance = nullptr;

framework::framework(HWND hwnd) : hwnd(hwnd)
{
	instance = this;
}

bool framework::initialize()
{
	// デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };

	UINT creat_device_flags{ 0 };
#ifdef _DEBUG
	creat_device_flags |= D3D11_CREATE_DEVICE_DEBUG;	// デバッグレイヤーをサポートするデバイスを作成
#endif
	// 各種設定
	{
		D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };	// 動作環境。これはDirectX11の基準を完全に満たしたGPUで動作するレベル

		DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
		swap_chain_desc.BufferCount = 1;	// SwapChainのバッファ数
		swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;	// バッファの横幅
		swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;	// バッファの縦幅
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// バッファのフォーマット指定 UNORMはUnsigned Normalizedの略、符号なし正規化整数らしい→ https://docs.microsoft.com/ja-jp/windows/uwp/graphics-concepts/data-type-conversion
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;	        // リフレッシュレートの設定 分母
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;	        // リフレッシュレートの設定 分子	つまり1/60
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使用方法の指定 シェーダの出力に使えるようにするらしい→ http://marupeke296.com/DX10_No1_Init.html
		swap_chain_desc.OutputWindow = hwnd;	// 出力ウィンドウハンドル
		swap_chain_desc.SampleDesc.Count = 1;	// 1ピクセルの色を決めるサンプリング数	未使用は1
		swap_chain_desc.SampleDesc.Quality = 0;	// サンプリングの品質(精度)				未使用は0
		swap_chain_desc.Windowed = !FULLSCREEN;	// ウィンドウモード
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creat_device_flags,
			&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, NULL, &immediate_context);	// DeviceとSwapChainの設定を同時に行う 参考→ https://yttm-work.jp/directx/directx_0012.html
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPRはおそらくメッセージ表示が可能なassert。SUCCEEDEDで成功判定、hr_traceはおそらくエラーメッセージの表示？

		// レンダーターゲットビューの設定
		ID3D11Texture2D* back_buffer{};
		hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));	// swap_chainのもつバッファで作成したいのでGetする
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		back_buffer->Release();	// Targetの取得が終わったのでバッファを解放。参照カウンタが１加算されているため解放しないとメモリリークの原因となる

		// 深度ステンシルビューの作成
		ID3D11Texture2D* depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC textuer2d_desc{};
		textuer2d_desc.Width = SCREEN_WIDTH;	// バッファの横幅
		textuer2d_desc.Height = SCREEN_HEIGHT;	// バッファの縦幅
		textuer2d_desc.MipLevels = 1;	// ミニマップのレベル指定
		textuer2d_desc.ArraySize = 1;	// テクスチャ配列のサイズ指定
		textuer2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// テクスチャのフォーマット。DXGI_FORMAT_D24_UNORM_S8_UINTを使うとDepth24bit、Stencil8bitとなる
		textuer2d_desc.SampleDesc.Count = 1;
		textuer2d_desc.SampleDesc.Quality = 0;
		textuer2d_desc.Usage = D3D11_USAGE_DEFAULT;				// テクスチャの使用方法
		textuer2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// バインド設定
		textuer2d_desc.CPUAccessFlags = 0;	// リソースへのCPUのアクセス権限設定 基本０でいいらしい
		textuer2d_desc.MiscFlags = 0;		// リソースオプションのフラグ 不必要なので０
		hr = device->CreateTexture2D(&textuer2d_desc, NULL, &depth_stencil_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = textuer2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;	// 最初に使用するミップマップのレベルを指定
		hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stensil_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		depth_stencil_buffer->Release();

		// サンプラーステートの生成
		D3D11_SAMPLER_DESC sampler_desc;
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// 拡縮時の色の取得方法 https://msdn.microsoft.com/ja-jp/library/ee416129(v=vs.85).aspx
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// UV座標が0～1の範囲外の場合の色の取得方法
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;							// 計算されたミップマップ レベルからのバイア
		sampler_desc.MaxAnisotropy = 16;						// サンプリングに異方性補間を使用している場合の限界値。有効な値は 1 ～ 16
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// 比較オプション
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;								// アクセス可能なミニマップの下限値
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;				// アクセス可能なミニマップの上限値
		hr = device->CreateSamplerState(&sampler_desc, &sampler_states[0]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		/*1-------------------------線形補間-------------------------1*/
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = device->CreateSamplerState(&sampler_desc, &sampler_states[1]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		/*2------------------------異方性補間------------------------2*/
		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device->CreateSamplerState(&sampler_desc, &sampler_states[2]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		// 深度ステンシルステートの生成
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	    // COMPARISON:比較	深度データの比較 今回は新規データが既存データ以下の場合に成功
		/*0-----------------------深度テスト:ON 深度ライト:ON-----------------------0*/
		depth_stencil_desc.DepthEnable = TRUE;	                        // 深度テストの有効/無効
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
		hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[0]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		/*1-----------------------深度テスト:ON 深度ライト:OFF-----------------------1*/
		depth_stencil_desc.DepthEnable = TRUE;							 // 深度テストの有効/無効
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
		hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[1]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		/*2-----------------------深度テスト:OFF 深度ライト:ON-----------------------2*/
		depth_stencil_desc.DepthEnable = FALSE;	                        // 深度テストの有効/無効
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
		hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[2]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		/*3-----------------------深度テスト:OFF 深度ライト:OFF-----------------------3*/
		depth_stencil_desc.DepthEnable = FALSE;	                         // 深度テストの有効/無効
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
		hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[3]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// ビューポートの設定
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(SCREEN_WIDTH);
		viewport.Height = static_cast<float>(SCREEN_HEIGHT);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediate_context->RSSetViewports(1, &viewport);

		// シーンコンスタントバッファの設定
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(scene_constants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// Blenderの設定
		blender.setBlender(device.Get());
	}
	// 各種クラス設定
	{
		// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
		sprites = make_unique<Sprite>(device.Get(), L".\\resources\\screenshot.jpg");	// シェーダーはコンストラクタ内で指定しているため、別を使うには改良が必要
		sprites->setSize(1280, 720);

		//sprite_batches[0] = make_unique<sprite_Batch>(device.Get(), L".\\resources\\player-sprites.png", 2048);

		sprite_text = make_unique<Sprite>(device.Get(), L".\\resources\\fonts\\font0.png");

		// Geometric_primitiveオブジェクトの生成
		{
			grid = make_unique<Geometric_Cube>(device.Get());
			grid->setPos(XMFLOAT3(0, -1, 0));
			grid->setSize(XMFLOAT3(10, 0.1f, 10));
		}
		obj_1= make_unique<Geometric_Capsule>(device.Get());
		obj_2= make_unique<Geometric_Capsule>(device.Get());

		// Static_Meshオブジェクトの生成
		static_mesh = make_unique<Static_Mesh>(device.Get(), L".\\resources\\Mr.Incredible\\Mr.Incredible.obj");
		skinned_mesh = make_unique<Skinned_Mesh>(device.Get(), ".\\resources\\cube.000.fbx");
	}
	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
	// imgui用変数

	// 2D用 内部関数で完結させてる？
	{
		sprites->ImguiWindow();
	}
	// 3D用パラメータ
	{
		//obj_1->imguiWindow("1");
		//obj_2->imguiWindow("2");
		static_mesh->imguiWindow("mesh");
	}

	// ライト調整等グローバル設定
	{
		ImGui::Begin("Light");
		ImGui::SliderFloat3("Light_Direction", light_dir, -10.0f, 10.0f);
		ImGui::End();

		// カメラ操作
		static float speed = 7.0f;
		if (GetKeyState('D') < 0)  eyePos.x += speed * elapsed_time;	// 右に
		if (GetKeyState('A') < 0)  eyePos.x -= speed * elapsed_time;	// 左に
		if (GetKeyState('W') < 0)  eyePos.z += speed * elapsed_time;	// 前に
		if (GetKeyState('S') < 0)  eyePos.z -= speed * elapsed_time;	// 後ろに
		if (GetKeyState(VK_SPACE) < 0)  eyePos.y += speed * elapsed_time;	// 上に
		if (GetKeyState(VK_SHIFT) < 0)  eyePos.y -= speed * elapsed_time;	// 下に

	}

#endif
}

void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr{ S_OK };

	FLOAT color[]{ 0.2f,0.2f,0.2f,1.0f };	// 背景色
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);	// クリア対象のView、クリアする色
	immediate_context->ClearDepthStencilView(depth_stensil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stensil_view.Get());

	// サンプラーステートをバインド
	immediate_context->PSSetSamplers(0, 1, sampler_states[0].GetAddressOf());
	immediate_context->PSSetSamplers(1, 1, sampler_states[1].GetAddressOf());
	immediate_context->PSSetSamplers(2, 1, sampler_states[2].GetAddressOf());

	immediate_context->OMSetDepthStencilState(depth_stencil_state[0].Get(), 1);	// バインドする深度ステンシルステート、参照値？
	immediate_context->OMSetBlendState(blender.states[Blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)

	// 2Dオブジェクトの描画設定
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_state[1].Get(), 1);	// 3Dオブジェクtの後ろに出すため一旦
		sprites->Render(immediate_context.Get());

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
		XMVECTOR focus{ XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
		XMVECTOR up{ XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
		// ViewMatrixの作成(LH = LeftHand(左手座標系))
		XMMATRIX V{ XMMatrixLookAtLH(eye, focus, up) };	// カメラ座標、焦点、カメラの上方向

		scene_constants data{};
		XMStoreFloat4x4(&data.view_projection, V * P);	// Matrixから4x4へ変換
		data.light_direction = { light_dir[0],light_dir[1],light_dir[2],0 };	// シェーダに渡すライトの向き
		data.camera_position = { eyePos.x,eyePos.y,eyePos.z,0 };				// シェーダに渡すカメラの位置
		immediate_context->UpdateSubresource(constant_buffer[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());	// cBufferはドローコールのたびに消去されるので都度設定する必要がある
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffer[0].GetAddressOf());

		immediate_context->OMSetDepthStencilState(depth_stencil_state[0].Get(), 1);			// 2Dオブジェクトとの前後関係をしっかりするため再設定

		{
			// geometric_primitiveに移植 現状必要なし？
			//XMMATRIX S{ XMMatrixScaling(geometric_primitive[0]->getSize().x,geometric_primitive[0]->getSize().y,geometric_primitive[0]->getSize().z) };				// 拡縮
			//XMMATRIX R{ XMMatrixRotationRollPitchYaw(geometric_primitive[0]->getAngle().x,geometric_primitive[0]->getAngle().y,geometric_primitive[0]->getAngle().z) };	// 回転
			//XMMATRIX T{ XMMatrixTranslation(geometric_primitive[0]->getPos().x,geometric_primitive[0]->getPos().y,geometric_primitive[0]->getPos().z) };			// 平行移動
			//XMFLOAT4X4 world;
			//XMStoreFloat4x4(&world, S * R * T);	// ワールド変換行列作成
			//geometric_primitive[0]->Render(immediate_context.Get(), world, geometric_primitive[0]->getColor());
			grid->wireframe = true;
			grid->Render(immediate_context.Get());
			//obj_1->Render(immediate_context.Get());
			//obj_2->Render(immediate_context.Get());
			static_mesh->Render(immediate_context.Get());
		}

	}


#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif


	UINT sync_interval{ 0 };				// sync_interval に 1 をセットすると 60FPS の固定フレームレートで動作する。０で可変フレームレートでの動作を前提に行う
	swap_chain->Present(sync_interval, 0);	// バックバッファをフロントバッファに送信する

}

bool framework::uninitialize()
{
	/*ComPtrに切り替えたのでRelease処理は必要なくなった*/
	//device->Release();
	//immediate_context->Release();
	//swap_chain->Release();
	//render_target_view->Release();
	//depth_stensil_view->Release();

	//for (auto& b : blender.states) {
	//	b->Release();
	//}

	//for (auto& dss : depth_stencil_state) {
	//	dss->Release();
	//}

	//for (Sprite* p : sprites) {
	//	delete p;
	//}

	return true;
}

framework::~framework()
{

}