#pragma once

//#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Object3d.h"
#include "geometric_primitive.h"

class Judge
{
public:


	Judge(){};
	~Judge();

	static bool c_b(const Object3d& capsle,const float cHeight,const float cRadian, const Object3d& sphere,const float sRadian);			// capsuleと球の当たり判定 高さや半径を指定して判定する
	static bool c_b(const Object3d& capsle,const Object3d& sphere);			// カプセルと球
	static bool s_s(const Object3d& shpere,const float shpereRadian, const Object3d& shpere2,const float shpere2Radian);	// 球と球の判定 そのまま
	static bool JudgeCap_Sphe(const Geometric_Capsule& capsle, const Geometric_Sphere& sphere);	// Geomteric_なんとかで判定を取る方法 総合的に大量に使うとまずいそうなので出番ない？
	static void HitJudge(Player* p, Enemy* e);

	static DirectX::XMFLOAT3 QuaternionToEulerAngles(const DirectX::XMFLOAT4& q);

	static Judge* getInstance()
	{
		static Judge* instance;
		return instance;
	}
};