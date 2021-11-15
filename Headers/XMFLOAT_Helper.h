#pragma once
#include <DirectXMath.h>

// ------------------------���Z�q�I�[�o�[���[�h------------------------

// XMFLOAT3 �n
static inline void              operator+= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; }

static inline void              operator+= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x += v2; v1.y += v2; v1.z += v2; }

static inline DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3 v2) { return DirectX::XMFLOAT3{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; }

static inline DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& v1, const float v2) { return DirectX::XMFLOAT3{ v1.x + v2, v1.y + v2, v1.z + v2 }; }

static inline void              operator-= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; }

static inline void              operator-= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x -= v2; v1.y -= v2; v1.z -= v2; }

static inline DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3 v2) { return DirectX::XMFLOAT3{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z }; }

static inline void              operator*= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x *= v2.x; v1.y *= v2.y; v1.z *= v2.z; }

static inline void              operator*= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x *= v2; v1.y *= v2; v1.z *= v2; }

static inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& v1, float v2) { return DirectX::XMFLOAT3{ v1.x * v2, v1.y * v2, v1.z * v2 }; }

static inline DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { return DirectX::XMFLOAT3{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z }; }

static inline void              operator/= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; }

static inline void              operator/= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x /= v2; v1.y /= v2; v1.z /= v2; }

// ------------------------�����v�Z ------------------------

// XMFLOAT3 �n

static inline float Float3Length(const DirectX::XMFLOAT3& v1) { return sqrtf(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z); }
// ------------------------���K�� ------------------------
static inline DirectX::XMFLOAT3 Float3Normalize(const DirectX::XMFLOAT3& v1)
{
	DirectX::XMVECTOR vector = DirectX::XMLoadFloat3(&v1);
	vector = DirectX::XMVector3Normalize(vector);
	DirectX::XMFLOAT3 N;
	DirectX::XMStoreFloat3(&N, vector);
	return N;
}
// ------------------------���� ------------------------

static inline float Float3Dot(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2) { return  v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }