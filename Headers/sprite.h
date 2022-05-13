#pragma once

#include "shaderEx.h"
#include "Object2d.h"
#include "texture.h"

#include <d3d11.h>
#include <SimpleMath.h>

#include <wrl.h>


class Sprite {
protected:
	// メンバ変数
	Microsoft::WRL::ComPtr<ID3D11Buffer>	VertexBuffer;

	static inline std::unique_ptr<ShaderEx> SpriteShader;	// デフォルトで使用するシェーダ
	std::unique_ptr<Texture> SpriteTexture;	// 画像を保持させている

	std::unique_ptr<Object2d> Parameter;

public:
	// コンストラクタ、デストラクタ
	Sprite();
	virtual ~Sprite();	// すべてのCOMオブジェクトを解放する

	// メンバ関数

	// 画像の読み込み
	void LoadImages(const wchar_t* filename);

	// 頂点情報の生成、更新
	void CreateVertexData(Texture* texture, Shader* shader);

	// メンバ変数のパラメータで描画
	void Render(Texture* texture = nullptr, Shader* shader = SpriteShader.get());	// immediate(対象となるデータそのものをコード中に記したものを即値という)

	//// 位置だけ指定するRender アニメーション不可
	//void Render(Shader* shader, DirectX::SimpleMath::Vector2 Pos, DirectX::SimpleMath::Vector2 Size);

	//// テキスト画像からテキストを切り抜いて描画(画像なのでフォーマット固定)
	//void Text_Out(ID3D11DeviceContext* immediate_context, std::string s, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color);

	// paramを編集するimguiウィンドウ
	void ImguiWindow();

	// XMFLOAT2同士の割り算
	DirectX::SimpleMath::Vector2 Division(DirectX::SimpleMath::Vector2 val1, DirectX::SimpleMath::Vector2 val2);

	// セッター
	void setPos    (DirectX::SimpleMath::Vector2 pos)     { Parameter->Pos     = pos; }
	void setSize(DirectX::SimpleMath::Vector2 Size)    { Parameter->Size    = Size; }
	void setTexPos (DirectX::SimpleMath::Vector2 texPos)  { Parameter->TexPos  = texPos; }
	void setTexSize(DirectX::SimpleMath::Vector2 texSize) { Parameter->TexSize = texSize; }
	void setRotate(float angle)      { Parameter->Angle   = angle; }
	void setColor  (DirectX::SimpleMath::Vector4 color)   { Parameter->Color   = color; }

	void setPos    (float posX, float posY)           { Parameter->Pos     = DirectX::SimpleMath::Vector2(posX, posY); }
	void setSize   (float SizeX, float SizeY)         { Parameter->Size    = DirectX::SimpleMath::Vector2(SizeX, SizeY); }
	void setTexPos (float texPosX, float texPosY)     { Parameter->TexPos  = DirectX::SimpleMath::Vector2(texPosX, texPosY); }
	void setTexSize(float texSizeX, float texSizeY)   { Parameter->TexSize = DirectX::SimpleMath::Vector2(texSizeX, texSizeY); }
	void setColor(float r, float g, float b, float a) { Parameter->Color   = DirectX::SimpleMath::Vector4(r, g, b, a); }

	// ゲッター
	DirectX::SimpleMath::Vector2 getPos()     { return Parameter->Pos;}
	DirectX::SimpleMath::Vector2 getSize()    { return Parameter->Size;}
	DirectX::SimpleMath::Vector2 getTexPos()  { return Parameter->TexPos;}
	DirectX::SimpleMath::Vector2 getTexSize() { return Parameter->TexSize;}
	float getAngle() { return Parameter->Angle; }
	DirectX::SimpleMath::Vector4 getColor  () { return Parameter->Color;}
	Texture* getTexture() { return SpriteTexture.get(); }
};

namespace SpriteMath
{
	// Render内で使う頂点回転用関数 sprite_Batchでも使用するのでclass Sprite外ヘッダーに記述
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

	// スクリーン座標系からNDC(正規化デバイス座標)への座標変換を行う
	// 矩形の左上のスクリーン座標と矩形サイズを渡す
	inline DirectX::SimpleMath::Vector3 ConvertToNDC(DirectX::SimpleMath::Vector3 pos, D3D11_VIEWPORT viewport) {
		pos.x = (pos.x * 2.0f / viewport.Width) - 1.0f;		// x値を２倍、その後スクリーンサイズで割って１を引くと正規化される
		pos.y = 1.0f - (pos.y * 2.0f / viewport.Height);	// y値を２倍、スクリーンサイズで割ったもので１を引くと正規化	xと違うのはおそらく左手右手座標系の関係
		// 今回はsprite(画像)なのでz値は変更する必要なし
		return pos;
	}
}