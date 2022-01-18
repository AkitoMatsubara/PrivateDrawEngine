#pragma once
#include <SimpleMath.h>

// 基本の3dオブジェクトに必要なパラメーター達
class Object2d
{
	// 変数
private:
public:
	DirectX::SimpleMath::Vector2 Pos;	// ワールド位置
	DirectX::SimpleMath::Vector2 Size;		// 大きさ
	DirectX::SimpleMath::Vector2 TexPos;	// テクスチャの開始位置
	DirectX::SimpleMath::Vector2 TexSize;	// テクスチャの使用サイズ
	float Angle;							// 回転値
	DirectX::SimpleMath::Vector4 Color;		// 色

	// その他 使ったり使わなかったりするやつ //
	bool Exist;		// 存在フラグ
	int MaxLife;	// 体力上限とか初期体力に
	int CurLife;	// 現在の(残り)体力
protected:

	// 関数
private:
public:
	// コンストラクタで初期化しておく 使うのにそもそも値入ってなかったらおかしいもんねぇ？
	Object2d()
	{
		Pos = DirectX::SimpleMath::Vector2{ 0.0f,0.0f };
		Size = DirectX::SimpleMath::Vector2{ 1.0f,1.0f };
		TexPos = DirectX::SimpleMath::Vector2{ 0.0f,0.0f };
		TexSize = DirectX::SimpleMath::Vector2{ 1.0f,1.0f };
		Angle = 0.0f;
		Color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f };

		Exist = true;
		MaxLife = 1;
		CurLife = MaxLife;
	};
	~Object2d() {};

	// Object2dのパラメータをコピーする
	void CopyParam(const Object2d* src) {
		Pos = src->Pos;
		Size = src->Size;
		TexPos = src->TexPos;
		TexSize = src->TexSize;
		Angle = src->Angle;
		Color = src->Color;

		Exist = src->Exist;
		MaxLife = src->MaxLife;
		CurLife = src->CurLife;
	}

protected:
};