
#pragma once

// UNIT.04
#include <d3d11.h>

class Texture
{
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	Texture();
	virtual ~Texture();
	bool Load(const wchar_t* filename = nullptr);
	bool LoadMipMap(const wchar_t* filename = nullptr);

	void Set(UINT Slot = 0, BOOL flg = TRUE);

	bool Create(u_int width, u_int height, DXGI_FORMAT format);
	bool CreateMipMap(u_int width, u_int height, DXGI_FORMAT format);
	// 深度ステンシルバッファ作成
	bool CreateDepth(u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);

	//シャドウ用ディプスバッファ
	bool CreateShadowDepth(u_int width, u_int height);


	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }

	ID3D11ShaderResourceView* GetRenderResourceView() { return ShaderResourceView.Get(); }

	ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
};
HRESULT load_texture_from_file(const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);
HRESULT make_dummy_texture(ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension);