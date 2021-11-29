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
	Aspect = 1280.0f / 720.0f;
	Near = 0.1f;
	Far = 2000.0f;
	Width = 1280.0f;
	Height = 720.0f;
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
		XMStoreFloat4x4(&ProjectionMatrix, pm);
	}
	else {
		pm = DirectX::XMMatrixOrthographicLH(Width, Height, Near, Far);
		XMStoreFloat4x4(&ProjectionMatrix, pm);
	}
}

void Camera::Operate()
{
	// カメラ操作
	static float cameraSpeed = 0.05f;
	static float cAngle = 0.0f;
	if (GetKeyState(VK_LEFT) < 0)   cAngle += DirectX::XMConvertToRadians(1);	// カメラ左回転
	if (GetKeyState(VK_RIGHT) < 0)  cAngle -= DirectX::XMConvertToRadians(1);	// カメラ右回転
	if (GetKeyState(VK_UP) < 0)     Position.y += cameraSpeed;	// 前に
	if (GetKeyState(VK_DOWN) < 0)   Position.y -= cameraSpeed;	// 後に
	if (GetKeyState(VK_SPACE) < 0)  Position.y += cameraSpeed;	// 上に
	if (GetKeyState(VK_SHIFT) < 0)  Position.y -= cameraSpeed;	// 下に
	Position.x = sinf(cAngle) * 10.0f;
	Position.z = cosf(cAngle) * 10.0f;

}
