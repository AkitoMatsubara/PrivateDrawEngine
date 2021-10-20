#pragma once
#include <DirectXMath.h>

// ------------------------演算子オーバーロード------------------------

// XMFLOAT3 系
static inline void operator+= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; }

static inline void operator-= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; }

static inline void operator*= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x *= v2.x; v1.y *= v2.y; v1.z *= v2.z; }

static inline void operator/= (DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; }

static inline void operator+= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x += v2; v1.y += v2; v1.z += v2; }

static inline void operator-= (DirectX::XMFLOAT3& v1, const float& v2) { v1.x -= v2; v1.y -= v2; v1.z -= v2; }


// ------------------------長さ計算 ------------------------

// XMFLOAT3 系

inline float Vector3Length(const DirectX::XMFLOAT3& v1)
{
	DirectX::XMFLOAT3 in_out_data{0};
	//XMFLOAT3を一度XMVECTORにロードする
	DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&v1);
	//計算する
	Vec = DirectX::XMVector3Length(Vec);
	XMStoreFloat3(&in_out_data, Vec);
	return in_out_data.x;
}

