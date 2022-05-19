
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

	void Set(ID3D11DeviceContext* device_context, UINT Slot = 0, BOOL flg = TRUE);

	bool Create(u_int width, u_int height, DXGI_FORMAT format);
	bool CreateMipMap(u_int width, u_int height, DXGI_FORMAT format);
	// 深度ステンシルバッファ作成
	bool CreateDepth(u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);

	//シャドウ用ディプスバッファ
	bool CreateShadowDepth(u_int width, u_int height);

	// フォント画像作成
	bool CreateFontImage(u_int width, u_int height,ID3D11Texture2D* texture2d );

	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }

	ID3D11ShaderResourceView* GetShaderResourceView() { return ShaderResourceView.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }

	static HRESULT LoadTextureFromFile(const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);
	static HRESULT MakeDummyTexture(ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension);
};