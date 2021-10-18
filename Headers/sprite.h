#pragma once
#include "shader.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

#include <wrl.h>


//// 頂点フォーマット
//struct Vertex {
//	XMFLOAT3 position;
//	XMFLOAT3 normal;
//	XMFLOAT2 texcoord;	// TextuerCoordinateの略、UV座標の取得などによく使われる様子
//	XMFLOAT4 color;
//};

// 矩形用のステータス
struct SpriteParam {
	DirectX::XMFLOAT2 Pos;		// 描画位置
	DirectX::XMFLOAT2 Size;		// 描画サイズ
	DirectX::XMFLOAT2 TexPos;	// テクスチャの開始位置
	DirectX::XMFLOAT2 TexSize;	// テクスチャの使用サイズ
	float Angle;		// 回転角度
	DirectX::XMFLOAT4 Color;		// 加算色
};

class Sprite {
private:
	// メンバ変数
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:片面塗りつぶし,1:片面ワイヤーフレーム,2:両面ワイヤーフレーム

	D3D11_TEXTURE2D_DESC	texture2d_desc;

	SpriteParam param;


	// 内部使用メンバ関数
	DirectX::XMFLOAT3 ConvertToNDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// コンストラクタ、デストラクタ
	Sprite(const wchar_t* filename, const char* vs_cso_name = "Shaders\\sprite_vs.cso", const char* ps_cso_name = "Shaders\\sprite_ps.cso");
	~Sprite();	// すべてのCOMオブジェクトを解放する

	// メンバ関数

	// 頂点情報の生成、更新
	void CreateVertexData(Shader* shader, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float angle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize);

	// メンバ変数でできるならいらない？と思い削除←やっぱいるわ課題的に
	void Render(Shader* shader, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 TexPos, DirectX::XMFLOAT2 TexSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// メンバ変数のパラメータで描画
	void Render(Shader* shader);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// 位置だけ指定するRender アニメーション不可
	void Render(Shader* shader, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size);

	//// テキスト画像からテキストを切り抜いて描画(画像なのでフォーマット固定)
	//void Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color);

	// paramを編集するimguiウィンドウ
	void ImguiWindow();

	// XMFLOAT2同士の割り算
	DirectX::XMFLOAT2 Division(DirectX::XMFLOAT2 val1, DirectX::XMFLOAT2 val2);


	// セッター
	void setPos    (DirectX::XMFLOAT2 pos)     { param.Pos     = pos; }
	void setSize   (DirectX::XMFLOAT2 Size)    { param.Size    = Size; }
	void setTexPos (DirectX::XMFLOAT2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(DirectX::XMFLOAT2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (DirectX::XMFLOAT4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)           { param.Pos     = DirectX::XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { param.Size    = DirectX::XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { param.TexPos  = DirectX::XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { param.TexSize = DirectX::XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { param.Color   = DirectX::XMFLOAT4(r, g, b, a); }

	// ゲッター
	DirectX::XMFLOAT2 getPos()     { return param.Pos;}
	DirectX::XMFLOAT2 getSize()    { return param.Size;}
	DirectX::XMFLOAT2 getTexPos()  { return param.TexPos;}
	DirectX::XMFLOAT2 getTexSize() { return param.TexSize;}
	float getAngle() { return param.Angle; }
	DirectX::XMFLOAT4 getColor  () { return param.Color;}
};

// render内で使う頂点回転用関数 sprite_Batchでも使用するのでclass Sprite外ヘッダーに記述
inline void rotate(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT2 center, float angle) {
	pos.x -= center.x;	// 一度中心点分ずらす
	pos.y -= center.y;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };	// DegreeなのでRadianに変換
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	float tx{ pos.x };	// 回転前の頂点座標
	float ty{ pos.y };
	pos.x = tx * cos - sin * ty;	// 回転の公式
	pos.y = tx * sin + cos * ty;

	pos.x += center.x;	// ずらした分戻す
	pos.y += center.y;
	}
