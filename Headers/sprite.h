#pragma once
#include "shader.h"
#include <d3d11.h>
#include <SimpleMath.h>
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
	DirectX::SimpleMath::Vector2 Pos;		// 描画位置
	DirectX::SimpleMath::Vector2 Size;		// 描画サイズ
	DirectX::SimpleMath::Vector2 TexPos;	// テクスチャの開始位置
	DirectX::SimpleMath::Vector2 TexSize;	// テクスチャの使用サイズ
	float Angle = 0.0f;		// 回転角度
	DirectX::SimpleMath::Vector4 Color;		// 加算色
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
	DirectX::SimpleMath::Vector3 ConvertToNDC(DirectX::SimpleMath::Vector3 val, D3D11_VIEWPORT viewport);

public:
	// コンストラクタ、デストラクタ
	Sprite(const wchar_t* filename);
	~Sprite();	// すべてのCOMオブジェクトを解放する

	// メンバ関数

	// 頂点情報の生成、更新
	void CreateVertexData(Shader* shader, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float angle, DirectX::SimpleMath::Vector4 color
		, DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);

	// メンバ変数でできるならいらない？と思い削除←やっぱいるわ課題的に
	void Render(Shader* shader, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float andle, DirectX::SimpleMath::Vector4 color
		, DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// メンバ変数のパラメータで描画
	void Render(Shader* shader);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// 位置だけ指定するRender アニメーション不可
	void Render(Shader* shader, DirectX::SimpleMath::Vector2 Pos, DirectX::SimpleMath::Vector2 Size);

	//// テキスト画像からテキストを切り抜いて描画(画像なのでフォーマット固定)
	//void Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color);

	// paramを編集するimguiウィンドウ
	void ImguiWindow();

	// XMFLOAT2同士の割り算
	DirectX::SimpleMath::Vector2 Division(DirectX::SimpleMath::Vector2 val1, DirectX::SimpleMath::Vector2 val2);


	// セッター
	void setPos    (DirectX::SimpleMath::Vector2 pos)     { param.Pos     = pos; }
	void setSize   (DirectX::SimpleMath::Vector2 Size)    { param.Size    = Size; }
	void setTexPos (DirectX::SimpleMath::Vector2 texPos)  { param.TexPos  = texPos; }
	void setTexSize(DirectX::SimpleMath::Vector2 texSize) { param.TexSize = texSize; }
	void setAngle  (float angle)      { param.Angle   = angle; }
	void setColor  (DirectX::SimpleMath::Vector4 color)   { param.Color   = color; }

	void setPos    (float posX, float posY)           { param.Pos     = DirectX::SimpleMath::Vector2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { param.Size    = DirectX::SimpleMath::Vector2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { param.TexPos  = DirectX::SimpleMath::Vector2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { param.TexSize = DirectX::SimpleMath::Vector2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { param.Color   = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// ゲッター
	DirectX::SimpleMath::Vector2 getPos()     { return param.Pos;}
	DirectX::SimpleMath::Vector2 getSize()    { return param.Size;}
	DirectX::SimpleMath::Vector2 getTexPos()  { return param.TexPos;}
	DirectX::SimpleMath::Vector2 getTexSize() { return param.TexSize;}
	float getAngle() { return param.Angle; }
	DirectX::SimpleMath::Vector4 getColor  () { return param.Color;}
};

// render内で使う頂点回転用関数 sprite_Batchでも使用するのでclass Sprite外ヘッダーに記述
inline void rotate(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector2 center, float angle) {
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
