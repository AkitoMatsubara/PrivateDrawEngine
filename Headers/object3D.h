#pragma once
#include <DirectXMath.h>

// 基本の3dオブジェクトに必要なパラメーター達
class Object3d
{
	// 変数
private:
public:
	DirectX::XMFLOAT3 Position;	    // ワールド位置
	DirectX::XMFLOAT3 Vector;		// 方向	というか現状前方方向を示している
	DirectX::XMFLOAT3 Acceleration; // 加速度
	DirectX::XMFLOAT3 Velocity;	    // 速度

	DirectX::XMFLOAT3 Scale;		// 大きさ
	DirectX::XMFLOAT3 Rotate;		// 各軸回転値
	DirectX::XMFLOAT4 Color;		// 色
protected:

// 関数
private:
public:
	// コンストラクタで初期化しておく
	Object3d()
	{
		Position     = { 0.0f,0.0f,0.0f };
		Vector       = { 0.0f,0.0f,0.0f };
		Acceleration = { 0.0f,0.0f,0.0f };
		Velocity     = { 0.0f,0.0f,0.0f };

		Scale        = { 0.0f,0.0f,0.0f };
		Rotate       = { 0.0f,0.0f,0.0f };
		Color        = { 0.0f,0.0f,0.0f,1.0f };
	};
	~Object3d() {};

	//// セッター
	//void setPos     (const DirectX::XMFLOAT3& pos)   { Position     = pos; }
	//void setVector  (const DirectX::XMFLOAT3& vec)   { Vector       = vec; };
	//void setAccel   (const DirectX::XMFLOAT3& accel) { Acceleration = accel; }
	//void setVelocity(const DirectX::XMFLOAT3& vel)   { Velocity     = vel; };
	//void setScale   (const DirectX::XMFLOAT3& scale) { Scale        = scale; }
	//void setRotate  (const DirectX::XMFLOAT3& angle) { Rotate       = angle; }
	//void setColor   (const DirectX::XMFLOAT4& color) { Color        = color; }



	//void setPos     (const float& x, const float& y, const float& z) { Position     = DirectX::XMFLOAT3(x, y, z); }
	//void setVector  (const float& x, const float& y, const float& z) { Vector       = DirectX::XMFLOAT3(x, y, z); };
	//void setAccel   (const float& x, const float& y, const float& z) { Acceleration = DirectX::XMFLOAT3(x, y, z); }
	//void setVelocity(const float& x, const float& y, const float& z) { Velocity     = DirectX::XMFLOAT3(x, y, z); };
	//void setScale   (const float& x, const float& y, const float& z) { Scale        = DirectX::XMFLOAT3(x, y, z); }
	//void setRotate  (const float& x, const float& y, const float& z) { Rotate       = DirectX::XMFLOAT3(x, y, z); }
	//void setColor   (const float& r, const float& g, const float& b, const float& a) { Color = DirectX::XMFLOAT4(r, g, b, a); }

	//// ゲッター
	//DirectX::XMFLOAT3 getPos()    { return Position; }
	//DirectX::XMFLOAT3 getScale()  { return Scale; }
	//DirectX::XMFLOAT3 getRotate() { return Rotate; }
	//DirectX::XMFLOAT4 getColor()  { return Color; }

	//const DirectX::XMFLOAT3 getVector()       { return Vector; };
	//const DirectX::XMFLOAT3 getAcceleration() { return Acceleration; }
	//const DirectX::XMFLOAT3 getVelocity()     { return Velocity; };


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