#include "framework.h"

framework::framework(HWND hwnd) : hwnd(hwnd)
{
}

bool framework::initialize()
{
	// デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };

	UINT creat_device_flags{ 0 };
#ifdef _DEBUG
	creat_device_flags |= D3D11_CREATE_DEVICE_DEBUG;	// デバッグレイヤーをサポートするデバイスを作成
#endif

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
	/*0-----------------------深度テスト:ON 深度ライト:ON-----------------------0*/
	depth_stencil_desc.DepthEnable = TRUE;	                        // 深度テストの有効/無効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	    // COMPARISON:比較	深度データの比較 今回は新規データが既存データ以下の場合に成功
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*1-----------------------深度テスト:ON 深度ライト:OFF-----------------------1*/
	depth_stencil_desc.DepthEnable = TRUE;							 // 深度テストの有効/無効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;		 // COMPARISON:比較	深度データの比較 今回は新規データが既存データ以下の場合に成功
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*2-----------------------深度テスト:OFF 深度ライト:ON-----------------------2*/
	depth_stencil_desc.DepthEnable = FALSE;	                        // 深度テストの有効/無効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	    // COMPARISON:比較	深度データの比較 今回は新規データが既存データ以下の場合に成功
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*3-----------------------深度テスト:OFF 深度ライト:OFF-----------------------3*/
	depth_stencil_desc.DepthEnable = FALSE;	                         // 深度テストの有効/無効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	     // COMPARISON:比較	深度データの比較 今回は新規データが既存データ以下の場合に成功
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

	// Blenderの設定
	blender.setBlender(device);

	// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
	sprites[0] = new Sprite(device, L".\\resources\\box.png");	// シェーダーはコンストラクタ内で指定しているため、別を使うには改良が必要

	sprites[1] = new Sprite(device, L".\\resources\\player-sprites.png");
	sprites[1]->setSize(1280, 720);

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
	static float pos[2]{ sprites[1]->getPos().x,sprites[1]->getPos().y };
	static float size[2]{ sprites[1]->getSize().x,sprites[1]->getSize().y };
	static float angle{ sprites[1]->getAngle() };
	static float TexPos[2]{ sprites[1]->getTexPos().x,sprites[1]->getTexPos().y };
	static float TexSize[2]{ sprites[1]->getTexSize().x,sprites[1]->getTexSize().y };
	static float Color[4] = { sprites[1]->getColor().x,sprites[1]->getColor().y,sprites[1]->getColor().z,sprites[1]->getColor().w };

	//----------------------

	ImGui::Begin("ImGUI");
		ImGui::SliderFloat2("Position", pos, -SCREEN_WIDTH, SCREEN_WIDTH);
		ImGui::SliderFloat2("Size", size, 0, 1960);
		ImGui::SliderFloat2("TexPos", TexPos, 0, 1960);
		ImGui::SliderFloat2("TexSize", TexSize, 0, 1960);
		ImGui::SliderFloat("angle", &angle, 0, 360);
		ImGui::ColorEdit4(u8"Color", (float*)&Color);

	ImGui::End();

	sprites[1]->setPos(DirectX::XMFLOAT2(pos[0], pos[1]));
	sprites[1]->setSize(DirectX::XMFLOAT2(size[0], size[1]));
	sprites[1]->setAngle(angle);
	sprites[1]->setTexPos(DirectX::XMFLOAT2(TexPos[0], TexPos[1]));
	sprites[1]->setTexSize(DirectX::XMFLOAT2(TexSize[0], TexSize[1]));
	sprites[1]->setColor(DirectX::XMFLOAT4(Color[0], Color[1], Color[2], Color[3]));

#endif
}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr{ S_OK };

	FLOAT color[]{ 0.2f,0.2f,0.2f,1.0f };	// 背景色
	immediate_context->ClearRenderTargetView(render_target_view, color);	// クリア対象のView、クリアする色
	immediate_context->ClearDepthStencilView(depth_stensil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stensil_view);

	// サンプラーステートをバインド
	immediate_context->PSSetSamplers(0, 1, &sampler_states[0]);
	immediate_context->PSSetSamplers(1, 1, &sampler_states[1]);
	immediate_context->PSSetSamplers(2, 1, &sampler_states[2]);

	// spritesの描画	(矩形)
	{
		immediate_context->OMSetBlendState(blender.states[Blender::BS_NONE], nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)
		immediate_context->OMSetDepthStencilState(depth_stencil_state[3], 1);	// バインドする深度ステンシルステート、参照値？
		sprites[0]->render(immediate_context, DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1280, 720), 0, DirectX::XMFLOAT4(1, 1, 1, 1)); //画像全体を画面全体に描画する
		immediate_context->OMSetBlendState(blender.states[Blender::BS_ALPHA], nullptr, 0xFFFFFFFF);	// ブレンドインターフェースのポインタ、ブレンドファクターの配列値、サンプルカバレッジ(今回はデフォルト指定)
		sprites[1]->render(immediate_context);
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
	device->Release();
	immediate_context->Release();
	swap_chain->Release();
	render_target_view->Release();
	depth_stensil_view->Release();

	for (auto& b : blender.states) {
		b->Release();
	}

	for (auto& dss : depth_stencil_state) {
		dss->Release();
	}

	for (Sprite* p : sprites) {
		delete p;
	}

	return true;
}

framework::~framework()
{

}