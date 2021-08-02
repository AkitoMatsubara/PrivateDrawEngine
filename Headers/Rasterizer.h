#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <assert.h>
using namespace Microsoft::WRL;

class Rasterizer {
public:

	enum RASTERIZER_STATE{
		SOLID_NONE,
		WIRE_NONE,
		SOLID_BACK,
		WIRE_BACK,
		RS_NONE,	// ラスタライザ終了
	};
	ComPtr<ID3D11RasterizerState>	states[RS_NONE];	// 0:片面塗りつぶし,1:両面塗り潰し,2:片面ワイヤーフレーム,3:両面ワイヤーフレーム

public:
	Rasterizer() {}
	Rasterizer(ID3D11Device* device) { SetRasterizer(device); }

	~Rasterizer() {}

	// RasterizerStateの作成
	void SetRasterizer(ID3D11Device* device) {
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
		/*-----塗り潰し 両面描画-----*/
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[SOLID_NONE].GetAddressOf());
		/*-----ワイヤーフレーム 両面描画-----*/
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[WIRE_NONE].GetAddressOf());
		/*-----塗りつぶし 前面描画-----*/
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;	// レンダリングに使う塗りつぶしモード D3D11_FILL_SOLID|D3D11_FILL_WIREFRAME
		rasterizer_desc.CullMode = D3D11_CULL_BACK;	    // 描画する法線方向 D3D11_CULL_NONE(両面描画)|D3D11_CULL_FRONT(後面描画)|D3D11_CULL_BACK(前面描画)
		rasterizer_desc.AntialiasedLineEnable = FALSE;	// MSAAのRTVを使用している時、線分描画でMultisampleEnableがfalseの時にアンチエイリアスを有効にする
		hr = device->CreateRasterizerState(&rasterizer_desc, states[SOLID_BACK].GetAddressOf());
		/*-----ワイヤーフレーム 前面描画-----*/
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[WIRE_BACK].GetAddressOf());

		if (SUCCEEDED(hr)) {
			assert("Create RasterizerState Failde.");
		}
	}
};