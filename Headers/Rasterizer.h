#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <assert.h>
using namespace Microsoft::WRL;

class Rasterizer {
public:

	enum RASTERIZER_STATE {
		SOLID_NONE,
		WIRE_NONE,
		SOLID_BACK,
		WIRE_BACK,
		SOLID_BACK_CCW,
		RS_NONE,	// ラスタライザ列挙型終了
	};
	ComPtr<ID3D11RasterizerState>	states[RS_NONE];	// 0:片面塗りつぶし,1:両面塗り潰し,2:片面ワイヤーフレーム,3:両面ワイヤーフレーム

public:
	Rasterizer() {}
	Rasterizer(ID3D11Device* device) { SetRasterizer(device); }

	~Rasterizer() {}

	// RasterizerStateの作成
	void SetRasterizer(ID3D11Device* device);
};