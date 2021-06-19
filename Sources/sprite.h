#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

// 頂点フォーマット
typedef struct VERTEX_FORMAT {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 texcoord;	// TextuerCoordinateの略、UV座標の取得などによく使われる様子
}vFormat_t;	// typedefの利点・変数宣言時に「struct VERTEX_FORMAT 変数名」と記述せず短く宣言できる

class Sprite {
private:
	// メンバ変数
	ID3D11VertexShader*			vertex_shader;
	ID3D11PixelShader*			pixel_shader;
	ID3D11InputLayout*			input_layout;
	ID3D11Buffer*				vertex_buffer;
	ID3D11ShaderResourceView*	shader_resource_view;
	D3D11_TEXTURE2D_DESC		texture2d_desc;


public:
	// コンストラクタ、デストラクタ
	Sprite(ID3D11Device* device, const wchar_t* filename);
	~Sprite();	// すべてのCOMオブジェクトを解放する

	// メンバ関数
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, float andle, DirectX::XMFLOAT4 color);	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// dx,dy＝矩形の左上のスクリーン座標、dw,dh＝矩形サイズ
	DirectX::XMFLOAT3 Convert_Screen_to_NDC(DirectX::XMFLOAT3 val, D3D11_VIEWPORT viewport);
};
