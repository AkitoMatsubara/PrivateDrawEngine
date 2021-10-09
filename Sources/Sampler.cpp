#include <d3d11.h>
#include "framework.h"
#include "Sampler.h"

Sampler::Sampler(D3D11_SAMPLER_DESC sd) {
	ID3D11Device* device = FRAMEWORK->GetDevice();

	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 拡縮時の色の取得方法 https://msdn.microsoft.com/ja-jp/library/ee416129(v=vs.85).aspx
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;		// UV座標が0～1の範囲外の場合の色の取得方法
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//FLOAT color[4] = { r,g,b,a };
	//sd.BorderColor = color;

	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device->CreateSamplerState(&sd, sampler_state.GetAddressOf());
	assert(SUCCEEDED(hr));

}

Sampler::Sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE Address_mode, FLOAT color[4]) {
	ID3D11Device* device = FRAMEWORK->GetDevice();
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = filter;			// 拡縮時の色の取得方法 https://msdn.microsoft.com/ja-jp/library/ee416129(v=vs.85).aspx
	sd.AddressU = Address_mode;	// UV座標が0～1の範囲外の場合の色の取得方法
	sd.AddressV = Address_mode;
	sd.AddressW = Address_mode;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	// colorが設定してあれば
	if (color) {
		memcpy(sd.BorderColor, color, sizeof(FLOAT) * 4);
	}

	sd.MinLOD = 0;					// アクセス可能なミニマップの下限値
	sd.MaxLOD = D3D11_FLOAT32_MAX;	// アクセス可能なミニマップの上限値

	HRESULT hr = device->CreateSamplerState(&sd, sampler_state.GetAddressOf());
	assert(SUCCEEDED(hr));

	//----------------------------------------------------------------
	//D3D11_SAMPLER_DESC sampler_desc;
	//sampler_desc.MipLODBias = 0;							// 計算されたミップマップ レベルからのバイアス
	//sampler_desc.MaxAnisotropy = 16;						// サンプリングに異方性補間を使用している場合の限界値。有効な値は 1 ～ 16
	//sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// 比較オプション
	//sampler_desc.BorderColor[0] = 0;
	//sampler_desc.BorderColor[1] = 0;
	//sampler_desc.BorderColor[2] = 0;
	//sampler_desc.BorderColor[3] = 0;
	//----------------------------------------------------------------
}

void Sampler::Set(UINT Slot)
{
	ID3D11DeviceContext* device_context = FRAMEWORK->GetDeviceContext();

	if (sampler_state) {
		device_context->PSSetSamplers(Slot, 1, sampler_state.GetAddressOf());
		device_context->DSSetSamplers(Slot, 1, sampler_state.GetAddressOf());
	}
}

