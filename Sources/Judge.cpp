#include "Judge.h"
#include "XMFLOAT_Helper.h"

bool Judge::c_b(const Object3d& capsle, const int height,const Object3d& sphere)
{
	DirectX::SimpleMath::Vector3 forward;	// カプセル上面の位置
	DirectX::SimpleMath::Vector3 back;		// カプセル下面の位置
	static float half = 1.5f;
	forward =	capsle.Position + (capsle.Vector * (capsle.Scale * height));	// 前方へズラせてカプセル上面として使う かけ数はサイズ的な意味合い
	back =		capsle.Position - (capsle.Vector * (capsle.Scale * height));	// 後方へズラせてカプセル下面として使う かけ数はサイズ的な意味合い

	DirectX::SimpleMath::Vector3 len1 = sphere.Position - forward;
	DirectX::SimpleMath::Vector3 len2 = sphere.Position - back;
	float d1 = len1.Length();	// 前方判定
	float d2 = len2.Length();	// 後方判定
	static const float RADIAN = 0.5f;	// 現在固定 うーん…
	if (d2 <= d1)
	{
		// スケールは全要素同数値なのでとりあえずどこでもいい
		if (d2 <= (RADIAN * capsle.Scale.x) || d2 <= (RADIAN * sphere.Scale.x)) {
			return true;
		}
	}
	else if (d1 <= d2)
	{
		if (d1 <= (RADIAN * capsle.Scale.x) || d1 <= (RADIAN * sphere.Scale.x)) {
			{
				return true;
			}
		}
	}
	return false;
};

bool Judge::JudgeCap_Sphe(const Geometric_Capsule& capsle, const Geometric_Sphere& sphere)
{
	DirectX::SimpleMath::Vector3 forward;	// カプセル上面の位置
	DirectX::SimpleMath::Vector3 back;		// カプセル下面の位置
	static float half = 1.5f;
	forward = capsle.Parameters->Position + (capsle.Parameters->Vector * (capsle.Parameters->Scale * (capsle.Height*0.5f)));	// 前方へズラせてカプセル上円点として使う かけ数はサイズ的な意味合い
	back =    capsle.Parameters->Position - (capsle.Parameters->Vector * (capsle.Parameters->Scale * (capsle.Height*0.5f)));	// 後方へズラせてカプセル下円点として使う かけ数はサイズ的な意味合い

	DirectX::SimpleMath::Vector3 len1 = sphere.Parameters->Position - forward;
	DirectX::SimpleMath::Vector3 len2 = sphere.Parameters->Position - back;
	float d1 = len1.LengthSquared();	// 前方判定
	float d2 = len2.LengthSquared();	// 後方判定

	if (d2 <= d1)
	{
		// sphereは球でスケールは全要素同数値なのでどこでもいい
		// なんで0.5倍したらいけたんや？わからん…
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