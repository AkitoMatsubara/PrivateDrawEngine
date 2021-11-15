#pragma once
#include <SimpleMath.h>

// 基本の3dオブジェクトに必要なパラメーター達
class Object3d
{
	// 変数
private:
public:
	DirectX::SimpleMath::Vector3 Position;	    // ワールド位置
	DirectX::SimpleMath::Vector3 Vector;		// 方向	というか現状前方方向を示している
	DirectX::SimpleMath::Vector3 Acceleration; // 加速度
	DirectX::SimpleMath::Vector3 Velocity;	    // 速度

	DirectX::SimpleMath::Vector3 Scale;		// 大きさ
	DirectX::SimpleMath::Vector3 Rotate;		// 各軸回転値
	DirectX::SimpleMath::Vector4 Color;		// 色
protected:

// 関数
private:
public:
	// コンストラクタで初期化しておく
	Object3d()
	{
		Position     = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Vector       = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Acceleration = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Velocity     = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };

		Scale        = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Rotate       = DirectX::SimpleMath::Vector3{ 0.0f,0.0f,0.0f };
		Color        = DirectX::SimpleMath::Vector4{ 0.0f,0.0f,0.0f,1.0f };
	};
	~Object3d() {};

	//// セッター
	//void setPos     (const DirectX::SimpleMath::Vector3& pos)   { Position     = pos; }
	//void setVector  (const DirectX::SimpleMath::Vector3& vec)   { Vector       = vec; };
	//void setAccel   (const DirectX::SimpleMath::Vector3& accel) { Acceleration = accel; }
	//void setVelocity(const DirectX::SimpleMath::Vector3& vel)   { Velocity     = vel; };
	//void setScale   (const DirectX::SimpleMath::Vector3& scale) { Scale        = scale; }
	//void setRotate  (const DirectX::SimpleMath::Vector3& angle) { Rotate       = angle; }
	//void setColor   (const DirectX::SimpleMath::Vector4& color) { Color        = color; }



	//void setPos     (const float& x, const float& y, const float& z) { Position     = DirectX::SimpleMath::Vector3(x, y, z); }
	//void setVector  (const float& x, const float& y, const float& z) { Vector       = DirectX::SimpleMath::Vector3(x, y, z); };
	//void setAccel   (const float& x, const float& y, const float& z) { Acceleration = DirectX::SimpleMath::Vector3(x, y, z); }
	//void setVelocity(const float& x, const float& y, const float& z) { Velocity     = DirectX::SimpleMath::Vector3(x, y, z); };
	//void setScale   (const float& x, const float& y, const float& z) { Scale        = DirectX::SimpleMath::Vector3(x, y, z); }
	//void setRotate  (const float& x, const float& y, const float& z) { Rotate       = DirectX::SimpleMath::Vector3(x, y, z); }
	//void setColor   (const float& r, const float& g, const float& b, const float& a) { Color = DirectX::SimpleMath::Vector4(r, g, b, a); }

	//// ゲッター
	//DirectX::SimpleMath::Vector3 getPos()    { return Position; }
	//DirectX::SimpleMath::Vector3 getScale()  { return Scale; }
	//DirectX::SimpleMath::Vector3 getRotate() { return Rotate; }
	//DirectX::SimpleMath::Vector4 getColor()  { return Color; }

	//const DirectX::SimpleMath::Vector3 getVector()       { return Vector; };
	//const DirectX::SimpleMath::Vector3 getAcceleration() { return Acceleration; }
	//const DirectX::SimpleMath::Vector3 getVelocity()     { return Velocity; };


	// Object3dのパラメータをコピーする
	void CopyParam(const Object3d* src) {
		Position     = src->Position;
		Vector       = src->Vector;
		Acceleration = src->Acceleration;
		Velocity     = src->Velocity;
		Scale        = src->Scale;
		Rotate       = src->Rotate;
		Color        = src->Color;
	}

protected:
};