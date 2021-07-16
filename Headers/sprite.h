#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;



// 頂点フォーマット
struct Vertex {
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;	// TextuerCoordinateの略、UV座標の取得などによく使われる様子
};

// 矩形用のステータス
struct SpriteParam {
	XMFLOAT2 Pos;		// 描画位置
	XMFLOAT2 Size;		// 描画サイズ
	XMFLOAT2 TexPos;	// テクスチャの開始位置
	XMFLOAT2 TexSize;	// テクスチャの使用サイズ
	float Angle;		// 回転角度
	XMFLOAT4 Color;		// 加算色
};

class Sprite {
private:
	// メンバ変数
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:片面塗りつぶし,1:片面ワイヤーフレーム,2:両面ワイヤーフレーム

	D3D11_TEXTURE2D_DESC	texture2d_desc;

	SpriteParam param;


	// 内部使用メンバ関数
	XMFLOAT3 ConvertToNDC(XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// コンストラクタ、デストラクタ
	Sprite(ID3D11Device* device, const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~Sprite();	// すべてのCOMオブジェクトを解放する

	// メンバ関数

	// 頂点情報の生成、更新
	void CreateVertexData(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float angle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);

	// メンバ変数でできるならいらない？と思い削除←やっぱいるわ課題的に
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 pos, XMFLOAT2 size, float andle, XMFLOAT4 color
		, XMFLOAT2 TexPos, XMFLOAT2 TexSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// メンバ変数のパラメータで描画
	void Render(ID3D11DeviceContext* immediate_context);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// 位置だけ指定するRender アニメーション不可
	void Render(ID3D11DeviceContext* immediate_context, XMFLOAT2 Pos, XMFLOAT2 Size);

	// テキスト画像からテキストを切り抜いて描画(画像なのでフォーマット固定)
	void Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color);

	// paramを編集するimguiウィンドウ
	void ImguiWindow();

	// XMFLOAT2同士の割り算
	XMFLOAT2 Division(XMFLOAT2 val1, XMFLOAT2 val2);


	// セッター
	void setPos    (XMFLOAT2 pos)     { param.Pos     = pos; }
	void setSize   (XMFLOAT2 Size)    { param.Size    = Size; }
	void setTexPos (XMFLOAT2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(XMFLOAT2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (XMFLOAT4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)           { param.Pos     = XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { param.Size    = XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { param.TexPos  = XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { param.TexSize = XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { param.Color   = XMFLOAT4(r, g, b, a); }

	// ゲッター
	XMFLOAT2 getPos()     { return param.Pos;}
	XMFLOAT2 getSize()    { return param.Size;}
	XMFLOAT2 getTexPos()  { return param.TexPos;}
	XMFLOAT2 getTexSize() { return param.TexSize;}
	float getAngle() { return param.Angle; }
	XMFLOAT4 getColor  () { return param.Color;}
};

// render内で使う頂点回転用関数 sprite_Batchでも使用するのでclass Sprite外ヘッダーに記述
inline void rotate(XMFLOAT3& pos, XMFLOAT2 center, float angle) {
	pos.x -= center.x;	// 一度中心点分ずらす
	pos.y -= center.y;

	float cos{ cosf(XMConvertToRadians(angle)) };	// DegreeなのでRadianに変換
	float sin{ sinf(XMConvertToRadians(angle)) };
	float tx{ pos.x };	// 回転前の頂点座標
	float ty{ pos.y };
	pos.x = tx * cos - sin * ty;	// 回転の公式
	pos.y = tx * sin + cos * ty;

	pos.x += center.x;	// ずらした分戻す
	pos.y += center.y;
	}
