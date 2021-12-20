#pragma once
#include <SimpleMath.h>

// 基本の3dオブジェクトに必要なパラメーター達
class Object3d
{
	// 変数
private:
public:
	DirectX::SimpleMath::Vector3 Position;		// ワールド位置
	DirectX::SimpleMath::Vector3 Vector;		// 方向	というか現状前方方向を示している
	DirectX::SimpleMath::Vector3 Acceleration;	// 加速度
	DirectX::SimpleMath::Vector3 Velocity;		// 速度

	DirectX::SimpleMath::Vector3 Scale;			// 大きさ
	//DirectX::SimpleMath::Vector3 Rotate;		// 各軸回転値
	DirectX::SimpleMath::Quaternion Orientation;// 回転行列 ちょっと管理難しくなった？
	DirectX::SimpleMath::Vector4 Color;			// 色

	// その他 使ったり使わなかったりするやつ //
	bool Exist;		// 存在フラグ
	int MaxLife;	// 体力上限とか初期体力に
	int CurLife;	// 現在の(残り)体力
protected:

	// 関数
private:
public:
	// コンストラクタで初期化しておく 使うのにそもそも値入ってなかったらおかしいもんねぇ？
	Object3d()
	{
		Position = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Vector = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Velocity = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };

		Scale = DirectX::SimpleMath::Vector3{ 1.0f,1.0f,1.0f };
		Orientation = DirectX::SimpleMath::Quaternion{ 0.0f,0.0f,0.0f,1.0f };
		Color = DirectX::SimpleMath::Vector4{ 0.0f,0.0f,0.0f,1.0f };

		Exist = true;
		MaxLife = 1;
		CurLife = MaxLife;
	};
	~Object3d() {};

	// 前方ベクトルを自己の回転角度から計算
	void calcForward()
	{
		Vector.y = sinf((Orientation.x));
		Vector.x = sinf((Orientation.y));
		Vector.z = cosf((Orientation.y));
		Vector.Normalize();	// 方向ベクトルなので正規化しとく
	}

	// Object3dのパラメータをコピーする
	void CopyParam(const Object3d* src) {
		Position = src->Position;
		Vector = src->Vector;
		Acceleration = src->Acceleration;
		Velocity = src->Velocity;
		Scale = src->Scale;
		Orientation = src->Orientation;
		Color = src->Color;
		Exist = src->Exist;
		MaxLife = src->MaxLife;
		CurLife = src->CurLife;
	}

protected:
};