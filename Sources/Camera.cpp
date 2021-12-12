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
	// �J��������
	static float cameraSpeed = 0.05f;
	static float cAngle = 0.0f;	// ��]�p
	static float cDist = -10.0f;	// ����
	static const float MIN_DIST = 3.0f;
	static const float MAX_DIST = 10.0f;
	if (GetKeyState(VK_LEFT) < 0)   cAngle += DirectX::XMConvertToRadians(1);	// �J��������]
	if (GetKeyState(VK_RIGHT) < 0)  cAngle -= DirectX::XMConvertToRadians(1);	// �J�����E��]

	if (GetKeyState(VK_SPACE) < 0)  Position.y += cameraSpeed;	// ���
	if (GetKeyState(VK_SHIFT) < 0)  Position.y -= cameraSpeed;	// ����

	DirectX::SimpleMath::Vector3 tg = Target - Position;	// �����x�N�g��
	tg.y = 0.0f;	// y�͏��O
	if (tg.Length() > MIN_DIST)	// �ŏ������`�F�b�N ����ȏ�͋߂Â��Ȃ�
	{
		if (GetKeyState(VK_UP) < 0)		cDist += 0.1f;	// �O��
	}
	if (tg.Length() < MAX_DIST) {	// �ő勗���`�F�b�N ����ȏ�͉�������Ȃ�
		if (GetKeyState(VK_DOWN) < 0)   cDist -= 0.1f;	// ���
	}
	tg.Normalize();	// �����Ȃ̂Ő��K��

	Position.z = cosf(cAngle) + (tg.z * cDist) + Target.z;
	Position.x = sinf(cAngle) + (tg.x * cDist) + Target.x;

}
