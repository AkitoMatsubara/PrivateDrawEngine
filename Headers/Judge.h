#pragma once

//#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Object3d.h"
#include "geometric_primitive.h"
// 当たり判定クラス いろんな判定を取れるようにしたいよね
class Judge
{
public:


	Judge(){};
	~Judge();

	static bool c_b(const Object3d& capsle,const float height, const Object3d& sphere);	// Object3dの値で頑張ろうとしてるやつ
	static bool JudgeCap_Sphe(const Geometric_Capsule& capsle, const Geometric_Sphere& sphere);	// Geomteric_[X]を使って判定しようとしたやつ
	static void HitJudge(Player* p, Enemy* e);	// 当たり判定を取る、ここで処理を行う

	static Judge* getInstance()
	{
		static Judge* instance;
		return instance;
	}
};