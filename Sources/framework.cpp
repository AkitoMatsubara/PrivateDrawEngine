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
	swap_chain_desc.SampleDesc.Quality = 0;	// サンプリングの品質(精度)			未使用は0
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

	// spriteオブジェクトを生成(今回は先頭の１つだけを生成する)
	sprites[0] = new Sprite(device);

	// ビューポートの設定
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);

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
	ImGui::Begin("ImGUI");
		ImGui::SliderFloat("Pos.x", &spritePos.x, 0, SCREEN_WIDTH);
		ImGui::SliderFloat("Pos.y", &spritePos.y, 0, SCREEN_HEIGHT);
		ImGui::SliderFloat("angle", &angle, 0, 360);
	ImGui::End();
#endif
}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr{ S_OK };

	FLOAT color[]{ 0.2f,0.2f,0.2f,1.0f };	// 背景色
	immediate_context->ClearRenderTargetView(render_target_view, color);	// クリア対象のView、クリアする色
	immediate_context->ClearDepthStencilView(depth_stensil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stensil_view);

	// spritesの描画	(矩形)
	// ポインタ、矩形左上の描画位置、矩形の大きさ、色
	//sprites[0]->render(immediate_context, spritePos.x,spritePos.y, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, DirectX::XMFLOAT4(0, 0.5f, 0, 1));
	sprites[0]->render(immediate_context, spritePos, DirectX::XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), angle, DirectX::XMFLOAT4(0, 0.5f, 0, 1));;

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif


	UINT sync_interval{ 0 };	// sync_interval に 1 をセットすると 60FPS の固定フレームレートで動作する。０で可変フレームレートでの動作を前提に行う
	swap_chain->Present(sync_interval, 0);	// バックバッファをフロントバッファに送信する

}

bool framework::uninitialize()
{
	device->Release();
	immediate_context->Release();
	swap_chain->Release();
	render_target_view->Release();
	depth_stensil_view->Release();

	// spritesオブジェクトの解放
	for (Sprite* p : sprites) {
		delete p;
	}

	return true;
}

framework::~framework()
{

}