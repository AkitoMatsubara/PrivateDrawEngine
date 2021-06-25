#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


// 頂点フォーマット
typedef struct VERTEX_FORMAT {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 texcoord;	// TextuerCoordinateの略、UV座標の取得などによく使われる様子
}vFormat_t;	// typedefの利点・変数宣言時に「struct VERTEX_FORMAT 変数名」と記述せず短く宣言できる

typedef struct STATUS {
	DirectX::XMFLOAT2 Pos;		// 描画位置
	DirectX::XMFLOAT2 Size;		// 描画サイズ
	DirectX::XMFLOAT2 TexPos;	// テクスチャの開始位置
	DirectX::XMFLOAT2 TexSize;	// テクスチャの使用サイズ
	float Angle;				// 回転角度
	DirectX::XMFLOAT4 Color;	// 加算色
}Status_t;	// 「_t」はtypedefで定義した構造体につける慣習がある：https://www.cc.kyoto-su.ac.jp/~yamada/programming/struct.html

class Sprite {
private:
	// メンバ変数
	ID3D11VertexShader*			vertex_shader;
	ID3D11PixelShader*			pixel_shader;
	ID3D11InputLayout*			input_layout;
	ID3D11Buffer*				vertex_buffer;
	ID3D11ShaderResourceView*	shader_resource_view;
	D3D11_TEXTURE2D_DESC		texture2d_desc;

	Status_t Status;


	// メンバ変数
	// dx,dy＝矩形の左上のスクリーン座標、dw,dh＝矩形サイズ
	DirectX::XMFLOAT3 ConvertToNDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// コンストラクタ、デストラクタ
	Sprite(ID3D11Device* device, const wchar_t* filename);
	~Sprite();	// すべてのCOMオブジェクトを解放する

	// メンバ関数
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// クリッピング可能render関数
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// クリッピング可能render関数 メンバ変数のパラメータで描画
	void render(ID3D11DeviceContext* immediate_context);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// Statusを編集するimguiウィンドウ
	void imguiWindow();

	// DirectX::XMFLOAT2同士の割り算
	DirectX::XMFLOAT2 division(DirectX::XMFLOAT2 val1, DirectX::XMFLOAT2 val2);

	// セッター
	void setPos    (DirectX::XMFLOAT2 pos)     { Status.Pos     = pos; }
	void setSize   (DirectX::XMFLOAT2 Size)    { Status.Size    = Size; }
	void setTexPos (DirectX::XMFLOAT2 texPos)  { Status.TexPos  = texPos; }
	void setTexSize(DirectX::XMFLOAT2 texSize) { Status.TexSize = texSize; }
	void setAngle  (float angle)               { Status.Angle   = angle; }
	void setColor  (DirectX::XMFLOAT4 color)   { Status.Color   = color; }

	void setPos    (float posX, float posY)           { Status.Pos     = DirectX::XMFLOAT2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { Status.Size    = DirectX::XMFLOAT2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { Status.TexPos  = DirectX::XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { Status.TexSize = DirectX::XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { Status.Color   = DirectX::XMFLOAT4(r, g, b, a); }

	// ゲッター
	DirectX::XMFLOAT2 getPos()     { return Status.Pos;}
	DirectX::XMFLOAT2 getSize()    { return Status.Size;}
	DirectX::XMFLOAT2 getTexPos()  { return Status.TexPos;}
	DirectX::XMFLOAT2 getTexSize() { return Status.TexSize;}
	float getAngle() { return Status.Angle; }
	DirectX::XMFLOAT4 getColor  () { return Status.Color;}
};
