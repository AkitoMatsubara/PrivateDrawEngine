#include "Judge.h"
#include "XMFLOAT_Helper.h"

bool Judge::c_b(const Object3d& capsle, const float cHeight, const float cRadian, const Object3d& sphere, const float sRadian)
{
	DirectX::SimpleMath::Vector3 forward;	// カプセル上面の位置
	DirectX::SimpleMath::Vector3 back;		// カプセル下面の位置
	DirectX::SimpleMath::Vector3 vec;
	vec = QuaternionToEulerAngles(capsle.Orientation);
	vec *= -1;
	vec.Normalize();
	forward = capsle.Position + (vec * (capsle.Scale * (cHeight * 0.5f)));	// 前方へズラせてカプセル上面として使う かけ数はサイズ的な意味合い
	back    = capsle.Position - (vec * (capsle.Scale * (cHeight * 0.5f)));	// 後方へズラせてカプセル下面として使う かけ数はサイズ的な意味合い

	DirectX::SimpleMath::Vector3 len1 = sphere.Position - forward;
	DirectX::SimpleMath::Vector3 len2 = sphere.Position - back;
	float d1 = len1.Length();	// 前方判定
	float d2 = len2.Length();	// 後方判定

	if (d1 == d2)	// 真横判定
	{
		if (d1 < capsle.Scale.x * cHeight) { return true; }
	}
	else if (d2 <= d1)
	{
		// スケールは全要素同数値なのでとりあえずどこでもいい
		if (d2 <= (capsle.Scale.x * cRadian) + (sphere.Scale.x * sRadian)+1) {
			return true;
		}
		//// スケールは全要素同数値なのでとりあえずどこでもいい
		//if (d2 > RADIAN * capsle.Scale.x && d2 > RADIAN * sphere.Scale.x) {
		//	return true;
		//}
	}
	else if (d1 <= d2)
	{
		if (d1 <= (capsle.Scale.x * cRadian) + (sphere.Scale.x * sRadian)+1) {
			{
				return true;
			}
		}
		//if (d1 > RADIAN * capsle.Scale.x || d1 > RADIAN * sphere.Scale.x) {
		//	{
		//		return true;
		//	}
		//}
	}
	return false;
};

bool Judge::c_b(const Object3d& capsle,const Object3d& sphere)
{
	const float sRadian = 0.5f;
	const float cHeight = 1.0f;
	const float cRadian = 0.5f;

	DirectX::SimpleMath::Vector3 forward;	// カプセル上面の位置
	DirectX::SimpleMath::Vector3 back;		// カプセル下面の位置
	DirectX::SimpleMath::Vector3 vec;
	vec = QuaternionToEulerAngles(capsle.Orientation);
	vec *= -1;
	vec.Normalize();
	forward = capsle.Position + (vec * (capsle.Scale * (cHeight * 0.5f)));	// 前方へズラせてカプセル上面として使う かけ数はサイズ的な意味合い
	back    = capsle.Position - (vec * (capsle.Scale * (cHeight * 0.5f)));	// 後方へズラせてカプセル下面として使う かけ数はサイズ的な意味合い

	DirectX::SimpleMath::Vector3 len1 = sphere.Position - forward;
	DirectX::SimpleMath::Vector3 len2 = sphere.Position - back;
	float d1 = len1.Length();	// 前方判定
	float d2 = len2.Length();	// 後方判定

	if (d1 == d2)	// 真横判定
	{
		if (d1 < capsle.Scale.x * cHeight) { return true; }
	}
	else if (d2 <= d1)
	{
		// スケールは全要素同数値なのでとりあえずどこでもいい
		if (d2 > cRadian * capsle.Scale.x && d2 > sRadian * sphere.Scale.x) {
			return true;
		}
	}
	else if (d1 <= d2)
	{
		if (d1 > cRadian * capsle.Scale.x || d1 > sRadian * sphere.Scale.x) {
			{
				return true;
			}
		}
	}
	return false;
};

bool Judge::s_s(const Object3d& shpere,const float shpereRadian, const Object3d& shpere2, const float shpere2Radian)
{
	DirectX::SimpleMath::Vector3 dist = shpere.Position - shpere2.Position;	// 球1と球2の差分(距離)
	float length = dist.Length();	// 長さ算出

	// 長さが球の半径より短ければ当たってる
	if (length < (shpereRadian * shpere.Scale.x) + (shpere2Radian * shpere2.Scale.x))
	{
		return true;
	}
	return false;
}


bool Judge::JudgeCap_Sphe(const Geometric_Capsule& capsle, const Geometric_Sphere& sphere)
{
	DirectX::SimpleMath::Vector3 forward;	// カプセル上面の位置
	DirectX::SimpleMath::Vector3 back;		// カプセル下面の位置

	forward = capsle.Parameters->Position + (capsle.Parameters->Vector * (capsle.Parameters->Scale * (capsle.Height * 0.5f)));	// 前方へズラせてカプセル上円点として使う かけ数はサイズ的な意味合い
	back = capsle.Parameters->Position - (capsle.Parameters->Vector * (capsle.Parameters->Scale * (capsle.Height * 0.5f)));	// 後方へズラせてカプセル下円点として使う かけ数はサイズ的な意味合い

	DirectX::SimpleMath::Vector3 len1 = forward - sphere.Parameters->Position;
	DirectX::SimpleMath::Vector3 len2 = back - sphere.Parameters->Position;
	float d1 = len1.LengthSquared();	// 前方判定
	float d2 = len2.LengthSquared();	// 後方判定

	if (d2 <= d1)
	{
		// sphereはスケールは全要素同数値なのでどこでもいい
		if (d2 <= (capsle.Radian * capsle.Parameters->Scale.x) || d2 <= (sphere.Radian * sphere.Parameters->Scale.x)) {
			return true;
		}
	}
	else if (d1 <= d2)
	{
		if (d1 <= (capsle.Radian * capsle.Parameters->Scale.x) || d1 <= (sphere.Radian * sphere.Parameters->Scale.x)) {
			{
				return true;
			}
		}
	}
	return false;
}

void Judge::HitJudge(Player* p, Enemy* e)
{
}

DirectX::XMFLOAT3 Judge::QuaternionToEulerAngles(const DirectX::XMFLOAT4& q)
{
	float q0q0 = q.x * q.x;
	float q0q1 = q.x * q.y;
	float q0q2 = q.x * q.z;
	float q0q3 = q.x * q.w;
	float q1q1 = q.y * q.y;
	float q1q2 = q.y * q.z;
	float q1q3 = q.y * q.w;
	float q2q2 = q.z * q.z;
	float q2q3 = q.z * q.w;
	float q3q3 = q.w * q.w;

	DirectX::XMFLOAT3 rot;
	rot.x = atan2(2.0f * (q2q3 + q0q1), q0q0 - q1q1 - q2q2 + q3q3);
	rot.y = asin(2.0f * (q0q2 - q1q3));
	rot.z = atan2(2.0f * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3);

	return rot;
}