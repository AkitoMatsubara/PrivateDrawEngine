#pragma once
#include "SimpleMath.h"

class Camera
{
private:
	float cAngle;	// ‰ñ“]Šp
	float cDist;	// ‹——£

	inline static constexpr float CAMMERASPEED = 0.05f;
protected:
	DirectX::SimpleMath::Vector3  Position;
	DirectX::SimpleMath::Vector3  Target;
	DirectX::SimpleMath::Vector3  Up;
	DirectX::SimpleMath::Matrix ViewMatrix;
	DirectX::SimpleMath::Matrix ProjectionMatrix;
	FLOAT Fov;
	FLOAT Aspect;
	FLOAT Near;	// •`‰æ”ÍˆÍ“I‚È
	FLOAT Far;	// •`‰æ”ÍˆÍ“I‚È
	FLOAT Width;
	FLOAT Height;
	bool bView;
public:
	Camera();
	virtual ~Camera();

	void Activate();	// ©g‚Ì’l‚ğ‚à‚Æ‚Éì¬‚·‚é
	void Operate();		// ƒJƒƒ‰‘€ì

	void SetPos(DirectX::XMFLOAT3 pos) { Position = pos; };
	void SetTarget(DirectX::XMFLOAT3 target) { Target = target; };
	void SetUp(DirectX::XMFLOAT3 up) { Up = up; };
	void Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);
	void SetProjection(FLOAT fov, FLOAT aspect, FLOAT min, FLOAT max);
	void SetOrtho(FLOAT width, FLOAT height, FLOAT min, FLOAT max);	// •½s“Š‰e

	DirectX::SimpleMath::Vector3 GetPos() { return Position; }
	DirectX::SimpleMath::Vector3 GetTarget() { return Target; }
	DirectX::SimpleMath::Vector3 GetUp() { return Up; }
	DirectX::XMMATRIX GetView() { return XMLoadFloat4x4(&ViewMatrix); }
	DirectX::XMMATRIX GetProjection() { return XMLoadFloat4x4(&ProjectionMatrix); }
	FLOAT GetNear() { return Near; }
	FLOAT GetFar() { return Far; }
	FLOAT GetWidth() { return Width; }
	FLOAT GetHeight() { return Height; }
	FLOAT GetAspext() { return Aspect; }

	static Camera& getInstance()
	{
		static Camera instance;
		return instance;
	}

};