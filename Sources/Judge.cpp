#include "Judge.h"
#include "XMFLOAT_Helper.h"

bool Judge::c_b(const Object3d& capsle, const Object3d& sphere)
{
	DirectX::SimpleMath::Vector3 forward;	// カプセル上面の位置
	forward = capsle.Position + (capsle.Vector * (capsle.Scale * 0.5f));	// 前方へズラせてカプセル上面として使う かけ数はサイズ的な意味合い
	DirectX::SimpleMath::Vector3 back;		// カプセル下面の位置
	back = capsle.Position - (capsle.Vector * (capsle.Scale * 0.5f));		// 後方へズラせてカプセル下面として使う かけ数はサイズ的な意味合い

	float d1 = Float3Length(forward - sphere.Position);	// 前方判定
	float d2 = Float3Length(back - sphere.Position);	// 後方判定

	if (d2 <= d1)
	{
		// sphereは球でスケールは全要素同数値なのでどこでもいい
		// なんで0.5倍したらいけたんや？わからん…
		if (d2 < (capsle.Scale.x + sphere.Scale.x * 0.5f)&& d2 < (capsle.Scale.y + sphere.Scale.y * 0.5f)&& d2 < (capsle.Scale.z + sphere.Scale.z * 0.5f))
		{
			return true;
		}
	}
	else if (d1 <= d2)
	{
		if (d1 < (capsle.Scale.x + sphere.Scale.x * 0.5f) && d2 < (capsle.Scale.y + sphere.Scale.y * 0.5f) && d2 < (capsle.Scale.z + sphere.Scale.z * 0.5f))
		{
			return true;
		}
	}
	return false;
};

void Judge::HitJudge(Player* p, Enemy* e)
{
}