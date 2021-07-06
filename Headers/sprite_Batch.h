#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "sprite.h"

// spriteと違いspriteBatchは複数の描画に対して一度しか読み込まなくて良い的な感じ
class sprite_Batch {
private:
	// メンバ変数
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;
	ID3D11Buffer* vertex_buffer;
	ID3D11ShaderResourceView* shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	Status_t Status{};

	// Sprite_Batch追加変数
	const size_t max_vertices;
	std::vector<vFormat_t> vertices;

	// 内部使用メンバ関数
	// dx,dy＝矩形の左上のスクリーン座標、dw,dh＝矩形サイズ
	DirectX::XMFLOAT3 ConvertToNDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);

public:
	// コンストラクタ、デストラクタ
	sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
	sprite_Batch(ID3D11Device* device, const wchar_t* filename);
	~sprite_Batch();	// すべてのCOMオブジェクトを解放する

	// メンバ関数
	void begin(ID3D11DeviceContext* immediate_context);
	void end(ID3D11DeviceContext* immediate_context);	// 頂点バッファの更新、各ステートのバインド、ドローコールを記述

		// クリッピング可能render関数	メンバ変数でできるならいらない？と思い削除
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color
		, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)


	// メンバ変数のパラメータで描画
	void render(ID3D11DeviceContext* immediate_context);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// Statusを編集するimguiウィンドウ
	void imguiWindow();

	// DirectX::XMFLOAT2同士の割り算
	DirectX::XMFLOAT2 division(DirectX::XMFLOAT2 val1, DirectX::XMFLOAT2 val2);

	// render内で使う頂点回転用関数
	void rotate(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT2 center, float angle);

	// セッター
	void setPos(DirectX::XMFLOAT2 pos) { Status.Pos = pos; }
	void setSize(DirectX::XMFLOAT2 Size) { Status.Size = Size; }
	void setTexPos(DirectX::XMFLOAT2 texPos) { Status.TexPos = texPos; }
	void setTexSize(DirectX::XMFLOAT2 texSize) { Status.TexSize = texSize; }
	void setAngle(float angle) { Status.Angle = angle; }
	void setColor(DirectX::XMFLOAT4 color) { Status.Color = color; }

	void setPos(float posX, float posY) { Status.Pos = DirectX::XMFLOAT2(posX, posY); }
	void setSize(float SizeX, float SizeY) { Status.Size = DirectX::XMFLOAT2(SizeX, SizeY); }
	void setTexPos(float texPosX, float texPosY) { Status.TexPos = DirectX::XMFLOAT2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY) { Status.TexSize = DirectX::XMFLOAT2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { Status.Color = DirectX::XMFLOAT4(r, g, b, a); }

	// ゲッター
	DirectX::XMFLOAT2 getPos() { return Status.Pos; }
	DirectX::XMFLOAT2 getSize() { return Status.Size; }
	DirectX::XMFLOAT2 getTexPos() { return Status.TexPos; }
	DirectX::XMFLOAT2 getTexSize() { return Status.TexSize; }
	float getAngle() { return Status.Angle; }
	DirectX::XMFLOAT4 getColor() { return Status.Color; }
};
