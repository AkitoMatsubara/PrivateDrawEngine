#include "framework.h"

#include "SceneGame.h"	// 初期シーンセット用 初期シーンには必須

#include "FrameRateCalculator.h"

#pragma comment(lib, "winmm.lib")	// timeGetTime()とか使うためwinmm.libを使用する

/* メモリリーク検出 newとかじゃないと反応しないよ
//======== メモリリークの検出器 =======//
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#define   malloc(s)             _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   calloc(c, s)          _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   realloc(p, s)         _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _recalloc(p, c, s)    _recalloc_dbg(p, c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _expand(p, s)         _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif
_CrtDumpMemoryLeaks();	// 呼び出し時、開放されていないポインタがあれば吐く
*/

framework* framework::instance = nullptr;

framework::framework(HWND hwnd) : hwnd(hwnd)
{
	instance = this;
}

bool framework::initialize()
{
	// デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };

	// 各種設定
	{
		CreateDeviceAndSwapCain();

		// レンダーターゲットビューの設定
		CreateRenderTargetView();

		// 深度ステンシルビューの作成
		CreateDepthStencileView();

		// サンプラーステートの生成
		CreateDepthStencileState();

		// ラスタライザステートの生成
		CreateRasterizerState();

		// RenderTargetの設定
		immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

		// ビューポートの設定
		CreateViewPort();

		// Blenderの設定
		CreateBlendState();
	}
	return true;
}

framework::~framework() {}

bool framework::uninitialize()
{
	/*ComPtrに切り替えたのでRelease処理は必要なくなった*/
	return true;
}

// デバイス,スワップチェーンの作成
bool framework::CreateDeviceAndSwapCain() {
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

	UINT creat_device_flags{ 0 };
#ifdef _DEBUG
	creat_device_flags |= D3D11_CREATE_DEVICE_DEBUG;	// デバッグレイヤーをサポートするデバイスを作成
#endif
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creat_device_flags,
		&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, NULL, &immediate_context);	// DeviceとSwapChainの設定を同時に行う 参考→ https://yttm-work.jp/directx/directx_0012.html
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	// _ASSERT_EXPRはおそらくメッセージ表示が可能なassert。SUCCEEDEDで成功判定、hr_traceはおそらくエラーメッセージの表示？

	return true;
}

// レンダーターゲットビューの設定
bool framework::CreateRenderTargetView() {
	ID3D11Texture2D* back_buffer{};
	HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));	// swap_chainのもつバッファで作成したいのでGetする
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(back_buffer, NULL, render_target_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();	// Targetの取得が終わったのでバッファを解放。参照カウンタが１加算されているため解放しないとメモリリークの原因となる

	return true;
}

// 深度ステンシルビューの作成
bool framework::CreateDepthStencileView() {
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC textuer2d_desc{};	// 深度ステンシル用テクスチャ
	textuer2d_desc.Width = SCREEN_WIDTH;	// バッファの横幅
	textuer2d_desc.Height = SCREEN_HEIGHT;	// バッファの縦幅
	textuer2d_desc.MipLevels = 1;	// ミニマップのレベル指定
	textuer2d_desc.ArraySize = 1;	// テクスチャ配列のサイズ指定
	textuer2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// テクスチャのフォーマット。DXGI_FORMAT_D24_UNORM_S8_UINTを使うとDepth24bit、Stencil8bitとなる
	textuer2d_desc.SampleDesc.Count = 1;
	textuer2d_desc.SampleDesc.Quality = 0;
	textuer2d_desc.Usage = D3D11_USAGE_DEFAULT;				// テクスチャの使用方法
	textuer2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// バインド設定 深度ステンシルターゲットとして設定
	textuer2d_desc.CPUAccessFlags = 0;	// リソースへのCPUのアクセス権限設定 基本０でいいらしい
	textuer2d_desc.MiscFlags = 0;		// リソースオプションのフラグ 不必要なので０
	HRESULT hr = device->CreateTexture2D(&textuer2d_desc, NULL, &depth_stencil_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = textuer2d_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;	// 最初に使用するミップマップのレベルを指定
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, &depth_stencil_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

// 深度ステンシルステートの生成
bool framework::CreateDepthStencileState() {
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
	/*-----------------------深度テスト:OFF 深度ライト:OFF-----------------------*/
	depth_stencil_desc.DepthEnable = FALSE;	                         // 深度テストの有効/無効 ここでは無効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度ステンシルバッファへの書き込みのOn/Off D3D11_DEPTH_WRITE_MASK_ALL|D3D11_DEPTH_WRITE_MASK_ZERO ここでは無効
	HRESULT hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[DS_FALSE]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	    // COMPARISON:比較	深度データの比較 今回は新規データが既存データ以下の場合に成功

	/*-----------------------深度テスト:ON 深度ライト:OFF-----------------------*/
	depth_stencil_desc.DepthEnable = TRUE;							 // 深度テストの有効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度ステンシルバッファへの書き込みの無効
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[DS_TRUE]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----------------------深度テスト:OFF 深度ライト:ON-----------------------*/
	depth_stencil_desc.DepthEnable = FALSE;	                        // 深度テストの無効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 深度ステンシルバッファへの書き込みの有効
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[DS_FALSE_WRITE]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	/*-----------------------深度テスト:ON 深度ライト:ON-----------------------*/
	depth_stencil_desc.DepthEnable = TRUE;	                        // 深度テストの有効
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 深度ステンシルバッファへの書き込みの有効
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state[DS_TRUE_WRITE]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

bool framework::CreateRasterizerState()
{
	HRESULT hr = { S_OK };

	// ラスタライザオブジェクトの生成
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FrontCounterClockwise = FALSE;	// 三角形が前面か背面かを決定する TRUEの時、頂点が反対周りだと前向きとみなされる
	rasterizer_desc.DepthBias = 0;					// 深度バイアス 同一深度上に表示するときに優先度を決めるのに使用したりする
	rasterizer_desc.DepthBiasClamp = 0;			    // 上記同様     ピクセルの最大深度バイアス
	rasterizer_desc.SlopeScaledDepthBias = 0;		// 上記同様     特定のピクセルの傾きのスカラー
	rasterizer_desc.DepthClipEnable = TRUE;		    // 距離に基づいてクリッピングを有効にするか
	rasterizer_desc.ScissorEnable = FALSE;			// シザー矩形カリングを使用するか シザー矩形：描画領域の指定によく使われる
	rasterizer_desc.MultisampleEnable = FALSE;		// マルチサンプリングアンチエイリアス(MSAA)のRTVを使用している時、tureで四辺形ラインアンチエイリアス、falseでアルファラインアンチエイリアスを使用
													// MSAAを使用するにはリソース生成時にDX11_SAMPLE_DESC::Countを1より上の値を設定する必要がある
	if (FAILED(hr))  assert("Create RasterizerState Failde.");
	/*-----塗り潰し 両面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[RS_SOLID_NONE].GetAddressOf());
	if (FAILED(hr))  assert("Create RasterizerState Failde.");
	/*-----ワイヤーフレーム 両面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[RS_WIRE_NONE].GetAddressOf());
	if (FAILED(hr))  assert("Create RasterizerState Failde.");
	/*-----塗りつぶし 前面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;	// レンダリングに使う塗りつぶしモード D3D11_FILL_SOLID|D3D11_FILL_WIREFRAME
	rasterizer_desc.CullMode = D3D11_CULL_BACK;	    // 描画する法線方向 D3D11_CULL_NONE(両面描画)|D3D11_CULL_FRONT(後面描画)|D3D11_CULL_BACK(前面描画)
	rasterizer_desc.AntialiasedLineEnable = FALSE;	// MSAAのRTVを使用している時、線分描画でMultisampleEnableがfalseの時にアンチエイリアスを有効にする
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[RS_SOLID_BACK].GetAddressOf());
	if (FAILED(hr))  assert("Create RasterizerState Failde.");
	/*-----ワイヤーフレーム 前面描画-----*/
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[RS_WIRE_BACK].GetAddressOf());
	if (FAILED(hr))  assert("Create RasterizerState Failde.");
	/*-----塗り潰し 前面描画 反時計回りの面-----*/
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	rasterizer_desc.FrontCounterClockwise = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[RS_SOLID_BACK_CCW].GetAddressOf());
	if (FAILED(hr))  assert("Create RasterizerState Failde.");
	return true;
}


// ブレンドステートの作成
bool framework::CreateBlendState() {
	HRESULT hr = { S_OK };

	D3D11_BLEND_DESC blend_desc{};

	/*----------[BS_NONE] なし----------*/
	blend_desc.AlphaToCoverageEnable = FALSE;	                                        // マスキングによりくりぬき処理を行ったポリゴンを不透明部分に対してのみ陰面処理に対応しつつレンダリングする手法？を有効にするか
	blend_desc.IndependentBlendEnable = FALSE;	                                        // 複数のRenderTarget[1～]を使用する場合はTrueに
	blend_desc.RenderTarget[0].BlendEnable = FALSE;	                                    // ブレンディングを有効にするかどうか
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;	                            // 最初のRGBデータソースの指定
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;	                        // 2番目のRGBデータソースの指定
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;	                        // RGBの組み合わせ方法を定義	今回はSrcBlendとDestBlendを加算
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;	                        // 最初のアルファデータソースを指定 ONEは(1,1,1,1)の白
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;	                    // 2番目のアルファデータソースを指定 ZEROは(0,0,0,0)の黒
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;	                    // アルファデータソースの組み合わせ方法を指定 加算
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// 書き込みマスク 今回はすべてのコンポーネントにデータを保存できるように
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_NONE].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	/*----------[BS_ALPHA] 透過----------*/
	blend_desc.AlphaToCoverageEnable = TRUE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;					// 今回はピクセルシェーダのアルファデータを指定、ブレンディング前の処理は無し
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;				// 今回はアルファデータ、ブレンディング前の処理によってデータが反転、１－Aが生成される
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;						// 今回はSrcBlendとDestBlendを加算
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;						// ONEは(1,1,1,1)の白
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;					// ZEROは(0,0,0,0)の黒
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;					// 加算
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_ALPHA].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	if (FAILED(hr))assert("ALPHA_BLEND ERROR");

	/*----------[BS_ADD] 加算----------*/
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_ADD].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	if (FAILED(hr))assert("ADD_BLEND ERROR");

	/*----------[BS_SUBTRACT] 減算----------*/
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_SUBTRACT].GetAddressOf());
	if (FAILED(hr))assert("NONE_BLEND ERROR");

	/*----------[BS_REPLACE]----------*/
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_REPLACE].GetAddressOf());
	if (FAILED(hr))assert("REPLACE_BLEND ERROR");

	/*----------[BS_MULTIPLY]----------*/
	ZeroMemory(&blend_desc, sizeof(blend_desc));
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_MULTIPLY].GetAddressOf());
	if (FAILED(hr))assert("MULTIPLY_BLEND ERROR");

	/*----------[BS_LIGHTEN]----------*/
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_LIGHTEN].GetAddressOf());
	if (FAILED(hr))assert("LIGHTEN_BLEND ERROR");

	/*----------[BS_DARKEN]----------*/
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, &bd_states[BS_DARKEN]);
	if (FAILED(hr))assert("DARKEN_BLEND ERROR");

	/*----------[BS_SCREEN]----------*/
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, bd_states[BS_SCREEN].GetAddressOf());
	if (FAILED(hr))assert("SCREEN_BLEND ERROR");

	// (ちなみに「Src」はSource(元)の略称らしい)
	// (SrcとDestで元と先を表す変数に使われたり？2要素って感じみたい)
	return true;
}

// ビューポートの作成
bool framework::CreateViewPort() {
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

// レンダーターゲットの初期化
void framework::Clear(FLOAT c[4]) {
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	immediate_context->ClearRenderTargetView(render_target_view.Get(), c);	// クリア対象のView、クリアする色
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetDepthStencilState(depth_stencil_state[DS_TRUE].Get(), 1);	// バインドする深度ステンシルステート、参照値？
}

void framework::Flip(int n) {
	swap_chain->Present(n, 0);	// バックバッファをフロントバッファに送信する
}

// ゲームループのおおもと
int framework::run() {
	MSG msg{};

	if (!initialize())	// framework初期化
	{
		return 0;
	}

#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
	ImGui::GetIO().Fonts->AddFontFromFileTTF("Resources\\fonts\\ipag.ttf", 15.0f, nullptr, glyphRangesJapanese);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device.Get(), immediate_context.Get());
	ImGui::StyleColorsDark();
#endif
	//[-------------------------------------------------------------------------------

	// メインループに入る前に精度を取得しておく
	//if (QueryPerformanceFrequency(&timeFreq) == FALSE) { // この関数で0(FALSE)が帰る時はハードウェア的に未対応(よっぽど古くない限り問題なはず)
	//	return static_cast<int>(msg.wParam); // 出来ないんでおわり
	//}
	//QueryPerformanceCounter(&timeStart); // 処理開始前に1度取得しておく(初回計算用)
	FrameRateCalculator::getInstance().Init();

	//-------------------------------------------------------------------------------]
	SceneManager::getInstance().ChangeScene(new SceneGame());

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			tictoc.tick();
			SceneManager::getInstance().Update();
			SceneManager::getInstance().Render();

#ifdef USE_IMGUI
			// フレームレート設定
			ImGui::Begin("fps Editor");
			if (ImGui::Button("15")) { FrameRateCalculator::getInstance().SetLimit(15);   FrameRateCalculator::getInstance().AveFrameTime = 0.0f; }	ImGui::SameLine();
			if (ImGui::Button("30")) { FrameRateCalculator::getInstance().SetLimit(30);   FrameRateCalculator::getInstance().AveFrameTime = 0.0f; }	ImGui::SameLine();
			if (ImGui::Button("60")) { FrameRateCalculator::getInstance().SetLimit(60);   FrameRateCalculator::getInstance().AveFrameTime = 0.0f; }	ImGui::SameLine();
			if (ImGui::Button("144")) { FrameRateCalculator::getInstance().SetLimit(144); FrameRateCalculator::getInstance().AveFrameTime = 0.0f; }	ImGui::SameLine();
			if (ImGui::Button("240")) { FrameRateCalculator::getInstance().SetLimit(240); FrameRateCalculator::getInstance().AveFrameTime = 0.0f; }	ImGui::SameLine();
			if (ImGui::Button("360")) { FrameRateCalculator::getInstance().SetLimit(360); FrameRateCalculator::getInstance().AveFrameTime = 0.0f; }
			ImGui::Text("fpsLimit:%d", FrameRateCalculator::getInstance().GetLimit());
			ImGui::Text("fps:%.2f", FrameRateCalculator::getInstance().fps);
			ImGui::Text("AverageFps:%.2f", FrameRateCalculator::getInstance().AveFrameTime);
			ImGui::End();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
			Flip();	// ImGui用

			// fps計算
			FrameRateCalculator::getInstance().Update();
			calculate_frame_stats();
		}
	}

	//delete scenemanager;	// 開放

#ifdef USE_IMGUI	// IMGUI後片付け
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

#if 1
	BOOL fullscreen = 0;
	swap_chain->GetFullscreenState(&fullscreen, 0);
	if (fullscreen)
	{
		swap_chain->SetFullscreenState(FALSE, 0);
	}
#endif
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK framework::handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_ENTERSIZEMOVE:
		tictoc.stop();
		break;
	case WM_EXITSIZEMOVE:
		tictoc.start();
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

void framework::calculate_frame_stats()
{
	if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::wostringstream outs;
		outs.precision(6);
		outs << APPLICATION_NAME << L" : Screen FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
		SetWindowTextW(hwnd, outs.str().c_str());
		frames = 0;
		elapsed_time += 1.0f;
	}
}