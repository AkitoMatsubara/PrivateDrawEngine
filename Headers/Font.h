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
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D; // 2D�e�N�X�`�� ���������[�N�΍��ComPtr�g�p
	//D3D11_TEXTURE2D_DESC	texture2d_desc;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	std::vector<const wchar_t*> FontList;	// �ǂݍ��񂾃t�H���g�̃p�X��ێ�����R���e�i
public:
	//ttf�t�@�C���ւ̃p�X
	Font(){};
	~Font();

	// �t�H���g�̓ǂݍ���
	void LoadFont(const wchar_t* ttfFontPath, const wchar_t* FontName);
	// �ꕶ��
	void CreateFontTexture(const WCHAR* FontChar);
};