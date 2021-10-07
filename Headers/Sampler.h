#pragma once

#include <wrl.h>	// ComPtr使用に必要
using namespace Microsoft::WRL;

class Sampler
{
protected:
	ComPtr<ID3D11SamplerState> sampler_state = nullptr;

public:
	// コンストラクタ1
	Sampler(D3D11_SAMPLER_DESC sd);

	// filter:
	//	D3D11_FILTER_MIN_MAG_MIP_LINEAR -線形補間
	//	D3D11_FILTER_ANISOTROPIC	    -異方性補間
	//	D3D11_FILTER_MIN_MAG_MIP_POINT  -ポイントサンプリング
	// Address_mode:
	//	D3D11_TEXTURE_ADDRESS_WRAP   -繰り返し
	//	D3D11_TEXTURE_ADDRESS_CLAMP  -引き伸ばし
	//	D3D11_TEXTURE_ADDRESS_MIRROR -鏡合わせ
	//	D3D11_TEXTURE_ADDRESS_BORDER -color[4]の色で塗りつぶす
	Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE Address_mode, FLOAT color[4] = { 0 });

	void Set(UINT Slot = 0);

	ID3D11SamplerState* GetSamplerState() { return sampler_state.Get(); }
};