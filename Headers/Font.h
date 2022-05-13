#pragma once
#include "sprite.h"

#include <d3d11.h>
#include <vector>
#include <map>
#include <string>


class Font :public Sprite
{
private:
	HFONT hFont;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D; // 2Dテクスチャ メモリリーク対策でComPtr使用
	//D3D11_TEXTURE2D_DESC	texture2d_desc;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	std::vector<const wchar_t*> FontList;	// 読み込んだフォントのパスを保持するコンテナ
public:
	//ttfファイルへのパス
	Font(){};
	~Font();

	// フォントの読み込み
	void LoadFont(const wchar_t* ttfFontPath, const wchar_t* FontName);
	// 一文字
	void CreateFontTexture(const WCHAR* FontChar);
};