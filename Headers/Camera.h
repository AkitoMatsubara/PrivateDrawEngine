#pragma once
#include "SimpleMath.h"

class Camera
{
private:
protected:
	DirectX::SimpleMath::Vector3  Position;
	DirectX::SimpleMath::Vector3  Target;
	DirectX::SimpleMath::Vector3  Up;
	DirectX::SimpleMath::Matrix ViewMatrix;
	DirectX::SimpleMath::Matrix ProjectionMatrix;
	FLOAT Fov;
	FLOAT Aspect;
	FLOAT Near;
	FLOAT Far;
	FLOAT Width;
	FLOAT Height;
	bool bView;

public:
	Camera();
	virtual ~Camera();
	void SetPos(DirectX::XMFLOAT3 pos) { Position = pos; };
	void SetTarget(DirectX::XMFLOAT3 target) { Target = target; };
	void SetUp(DirectX::XMFLOAT3 up) { Up = up; };
	void Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);
	void SetProjection(FLOAT fov, FLOAT aspect, FLOAT min, FLOAT max);
	void SetOrtho(FLOAT width, FLOAT height, FLOAT min, FLOAT max);

	void Activate();	// é©êgÇÃílÇÇ‡Ç∆Ç…çÏê¨Ç∑ÇÈ
	void Operate();
	DirectX::XMMATRIX GetView() { return XMLoadFloat4x4(&ViewMatrix); }
	DirectX::XMMATRIX GetProjection() { return XMLoadFloat4x4(&ProjectionMatrix); }
	DirectX::SimpleMath::Vector3 GetPos() { return Position; }
	DirectX::SimpleMath::Vector3 GetTarget() { return Target; }
	FLOAT GetFar() { return Far; }
	FLOAT GetNear() { return Near; }
	FLOAT GetWidth() { return Width; }
	FLOAT GetHeight() { return Height; }
	FLOAT GetAspext() { return Aspect; }

};