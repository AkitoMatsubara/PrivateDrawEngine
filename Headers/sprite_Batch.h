#pragma once

#include "sprite.h"
#include "shaderEx.h"
#include "Object2d.h"

#include <vector>


// spriteと違いspriteBatchは複数の描画に対して一度しか読み込まなくて良い的な感じ
class sprite_Batch {
private:
	// メンバ変数
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizer_states[3];	// 0:片面塗りつぶし,1:片面ワイヤーフレーム,2:両面ワイヤーフレーム

	D3D11_TEXTURE2D_DESC	texture2d_desc;

	//SpriteParam Parameters{};
	std::unique_ptr<Object2d> Parameters;
	static inline std::unique_ptr<ShaderEx> SpriteShader;	// デフォルトで使用するシェーダ

	// Sprite_Batch追加変数
	const size_t max_vertices;
	std::vector<Vertex> vertices;
public:
	// コンストラクタ、デストラクタ
	sprite_Batch(const wchar_t* filename, size_t max_sprites = 1);
	~sprite_Batch();	// すべてのCOMオブジェクトを解放する

	// メンバ関数
	void begin();
	void end(ShaderEx* shader = SpriteShader.get());	// 頂点バッファの更新、各ステートのバインド、ドローコールを記述

	// 頂点情報の生成、更新
	void CreateVertexData(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float angle, DirectX::SimpleMath::Vector4 color
		, DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);

	// メンバ変数でできるならいらない？と思い削除←やっぱいるわ課題的に
	void Render(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, float andle, DirectX::SimpleMath::Vector4 color
		, DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);	// immediate(対象となるデータそのものをコード中に記したものを即値という)


	// メンバ変数のパラメータで描画
	void Render();	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	// テクスチャ位置だけ指定するRender 課題的に(ry
	void Render(DirectX::SimpleMath::Vector2 TexPos, DirectX::SimpleMath::Vector2 TexSize);

	// セッター
	void setPos    (DirectX::SimpleMath::Vector2 pos)     { Parameters->Pos     = pos; }
	void setSize   (DirectX::SimpleMath::Vector2 Size)    { Parameters->Size    = Size; }
	void setTexPos (DirectX::SimpleMath::Vector2 texPos)  { Parameters->TexPos  = texPos; }
	void setTexSize(DirectX::SimpleMath::Vector2 texSize) { Parameters->TexSize = texSize; }
	void setAngle  (float angle)      { Parameters->Angle   = angle; }
	void setColor  (DirectX::SimpleMath::Vector4 color)   { Parameters->Color   = color; }

	void setPos    (float posX, float posY)             { Parameters->Pos     = DirectX::SimpleMath::Vector2(posX, posY); }
	void setSize   (float SizeX, float SizeY)           { Parameters->Size    = DirectX::SimpleMath::Vector2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)       { Parameters->TexPos  = DirectX::SimpleMath::Vector2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)     { Parameters->TexSize = DirectX::SimpleMath::Vector2(texSizeX, texSizeY); }
	void setColor  (float r, float g, float b, float a) { Parameters->Color   = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// ゲッター
	DirectX::SimpleMath::Vector2 getPos()     { return Parameters->Pos; }
	DirectX::SimpleMath::Vector2 getSize()    { return Parameters->Size; }
	DirectX::SimpleMath::Vector2 getTexPos()  { return Parameters->TexPos; }
	DirectX::SimpleMath::Vector2 getTexSize() { return Parameters->TexSize; }
	float	 getAngle()   { return Parameters->Angle; }
	DirectX::SimpleMath::Vector4 getColor()   { return Parameters->Color; }
};
