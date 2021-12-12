#include "Camera.h"

Camera::Camera()
{
	Position = DirectX::SimpleMath::Vector3{ 0,1,10 };
	Target = DirectX::SimpleMath::Vector3{ 0,0,0 };
	Up = DirectX::SimpleMath::Vector3{ 0,1,0 };
	ViewMatrix = DirectX::SimpleMath::Matrix{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };
	ProjectionMatrix = DirectX::SimpleMath::Matrix{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };
	Fov = DirectX::XM_PI / 6.0f;
	Near = 0.1f;
	Far = 2000.0f;
	Width = 1280.0f;
	Height = 720.0f;
	Aspect = Width / Height;
	bView = false;
}
Camera::~Camera(){}

void Camera::Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up)
{
	Position = pos;
	Target = target;
	Up = up;
}

void Camera::SetProjection(FLOAT fov, FLOAT aspect, FLOAT min, FLOAT max)
{
	bView = true;
	Fov = fov;
	Aspect = aspect;
	Near = min;
	Far = max;
}

void Camera::SetOrtho(FLOAT width, FLOAT height, FLOAT min, FLOAT max)
{
	bView = false;
	Width = width;
	Height = height;
	Near = min;
	Far = max;
}

void Camera::Activate()
{
	DirectX::XMVECTOR cPos, cTarget, cUp;
	cPos = DirectX::XMVectorSet(Position.x, Position.y, Position.z, 0);
	cTarget = DirectX::XMVectorSet(Target.x, Target.y, Target.z, 0);
	cUp = DirectX::XMVectorSet(Up.x, Up.y, Up.z, 0);

	DirectX::XMMATRIX vm = DirectX::XMMatrixLookAtLH(cPos, cTarget, cUp);
	DirectX::XMStoreFloat4x4(&ViewMatrix, vm);

	DirectX::XMMATRIX pm = DirectX::XMMatrixIdentity();
	if (bView) {
		pm = DirectX::XMMatrixPerspectiveFovLH(Fov, Aspect, Near, Far);
		DirectX::XMStoreFloat4x4(&ProjectionMatrix, pm);
	}
	else {
		pm = DirectX::XMMatrixOrthographicLH(Width, Height, Near, Far);
		DirectX::XMStoreFloat4x4(&ProjectionMatrix, pm);
	}
}

void Camera::Operate()
{
	// カメラ操作
	static float cameraSpeed = 0.05f;
	static float cAngle = 0.0f;	// 回転角
	static float cDist = -10.0f;	// 距離
	static const float MIN_DIST = 3.0f;
	static const float MAX_DIST = 10.0f;
	if (GetKeyState(VK_LEFT) < 0)   cAngle += DirectX::XMConvertToRadians(1);	// カメラ左回転
	if (GetKeyState(VK_RIGHT) < 0)  cAngle -= DirectX::XMConvertToRadians(1);	// カメラ右回転

	if (GetKeyState(VK_SPACE) < 0)  Position.y += cameraSpeed;	// 上に
	if (GetKeyState(VK_SHIFT) < 0)  Position.y -= cameraSpeed;	// 下に

	DirectX::SimpleMath::Vector3 tg = Target - Position;	// 方向ベクトル
	tg.y = 0.0f;	// yは除外
	if (tg.Length() > MIN_DIST)	// 最小距離チェック これ以上は近づけない
	{
		if (GetKeyState(VK_UP) < 0)		cDist += 0.1f;	// 前に
	}
	if (tg.Length() < MAX_DIST) {	// 最大距離チェック これ以上は遠ざかれない
		if (GetKeyState(VK_DOWN) < 0)   cDist -= 0.1f;	// 後に
	}
	tg.Normalize();	// 方向なので正規化

	Position.z = cosf(cAngle) + (tg.z * cDist) + Target.z;
	Position.x = sinf(cAngle) + (tg.x * cDist) + Target.x;

}
