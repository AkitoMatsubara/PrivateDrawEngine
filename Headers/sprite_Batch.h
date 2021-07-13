#pragma once
//#include <d3d11.h>
//#include <DirectXMath.h>

//#include "imgui.h"
//#include "imgui_impl_dx11.h"
//#include "imgui_impl_win32.h"

#include "sprite.h"
#include <vector>


// spriteと違いspriteBatchは複数の描画に対して一度しか読み込まなくて良い的な感じ
class sprite_Batch {
private:
	// メンバ変数
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;

	D3D11_TEXTURE2D_DESC				texture2d_desc;

	SpriteParam param{};

	// Sprite_Batch追加変数
	const size_t max_vertices;
	std::vector<Vertex> vertices;

	// 内部使用メンバ関数
	// dx,dy＝矩形の左上のスクリーン座標、dw,dh＝矩形サイズ
	XMFLOAT3 ConvertToNDC(XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// コンストラクタ、デストラクタ
	sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites = 1, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	sprite_Batch(ID3D11Device* device, const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~sprite_Batch();	// すべてのCOMオブジェクトを解放する

	// メンバ関数
	void begin(ID3D11DeviceContext* immediate_context);
	void end(ID3D11DeviceContext* immediate_context);	// 頂点バッファの更新、各ステートのバインド、ドローコールを記述

	// 頂点情報の生成、更新
	void CreateVertexData(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float angle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// メンバ変数でできるならいらない？と思い削除←やっぱいるわ課題的に
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float andle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)


	// メンバ変数のパラメータで描画
	void Render(ID3D11DeviceContext* immediate_context);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

		// テクスチャ位置だけ指定するRender 課題的に(ry
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// Statusを編集するimguiウィンドウ
	void imguiWindow();

	// XMFLOAT2同士の割り算
	XMFLOAT2 division(XMFLOAT2 val1, XMFLOAT2 val2);

	// セッター
	void setPos    (XMFLOAT2 pos)     { param.Pos     = pos; }
	void setSize   (XMFLOAT2 Size)    { param.Size    = Size; }
	void setTexPos (XMFLOAT2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(XMFLOAT2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (XMFLOAT4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)             { param.Pos     = XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)           { param.Size    = XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)       { param.TexPos  = XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)     { param.TexSize = XMFLOAT2(texSizeX, texSizeY); }
	void setColor  (float r, float g, float b, float a) { param.Color   = XMFLOAT4(r, g, b, a); }

	// ゲッター
	XMFLOAT2 getPos()     { return param.Pos; }
	XMFLOAT2 getSize()    { return param.Size; }
	XMFLOAT2 getTexPos()  { return param.TexPos; }
	XMFLOAT2 getTexSize() { return param.TexSize; }
	float	 getAngle()   { return param.Angle; }
	XMFLOAT4 getColor()   { return param.Color; }
};
