#pragma once

#include "sprite.h"

#include <vector>


// spriteと違いspriteBatchは複数の描画に対して一度しか読み込まなくて良い的な感じ
class sprite_Batch {
private:
	// メンバ変数
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:片面塗りつぶし,1:片面ワイヤーフレーム,2:両面ワイヤーフレーム

	D3D11_TEXTURE2D_DESC				texture2d_desc;

	SpriteParam param{};

	// Sprite_Batch追加変数
	const size_t max_vertices;
	std::vector<Vertex> vertices;

	// 内部使用メンバ関数
	// dx,dy＝矩形の左上のスクリーン座標、dw,dh＝矩形サイズ
	DirectX::XMFLOAT3 ConvertToNDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// コンストラクタ、デストラクタ
	sprite_Batch(const wchar_t* filename, size_t max_sprites = 1, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	sprite_Batch(const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~sprite_Batch();	// すべてのCOMオブジェクトを解放する

	// メンバ関数
	void begin();
	void end();	// 頂点バッファの更新、各ステートのバインド、ドローコールを記述

	// 頂点情報の生成、更新
	void CreateVertexData(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize);

	// メンバ変数でできるならいらない？と思い削除←やっぱいるわ課題的に
	void Render(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)


	// メンバ変数のパラメータで描画
	void Render();	// immediate(対象となるデータそのものをコード中に記したものを即値という)

		// テクスチャ位置だけ指定するRender 課題的に(ry
	void Render(DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize);

	// セッター
	void setPos    (DirectX::XMFLOAT2 pos)     { param.Pos     = pos; }
	void setSize   (DirectX::XMFLOAT2 Size)    { param.Size    = Size; }
	void setTexPos (DirectX::XMFLOAT2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(DirectX::XMFLOAT2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (DirectX::XMFLOAT4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)             { param.Pos     = DirectX::XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)           { param.Size    = DirectX::XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)       { param.TexPos  = DirectX::XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)     { param.TexSize = DirectX::XMFLOAT2(texSizeX, texSizeY); }
	void setColor  (float r, float g, float b, float a) { param.Color   = DirectX::XMFLOAT4(r, g, b, a); }

	// ゲッター
	DirectX::XMFLOAT2 getPos()     { return param.Pos; }
	DirectX::XMFLOAT2 getSize()    { return param.Size; }
	DirectX::XMFLOAT2 getTexPos()  { return param.TexPos; }
	DirectX::XMFLOAT2 getTexSize() { return param.TexSize; }
	float	 getAngle()   { return param.Angle; }
	DirectX::XMFLOAT4 getColor()   { return param.Color; }
};
