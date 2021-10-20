#pragma once
#include <DirectXMath.h>

// ------------------------���Z�q�I�[�o�[���[�h------------------------

// XMFLOAT3 �n
static inline void operator+= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; }

static inline void operator-= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; }

static inline void operator*= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x *= v2.x; v1.y *= v2.y; v1.z *= v2.z; }

static inline void operator/= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; }

static inline void operator+= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x += v2; v1.y += v2; v1.z += v2; }

static inline void operator-= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x -= v2; v1.y -= v2; v1.z -= v2; }


// ------------------------�����v�Z ------------------------

// XMFLOAT3 �n

inline float Vector3Length(const DirectX::XMFLOAT3& v1)
{
	DirectX::XMFLOAT3 in_out_data{0};
	//XMFLOAT3����xXMVECTOR�Ƀ��[�h����
	DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&v1);
	//�v�Z����
	Vec = DirectX::XMVector3Length(Vec);
	XMStoreFloat3(&in_out_data, Vec);
	return in_out_data.x;
}

