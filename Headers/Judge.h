#pragma once

//#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Object3d.h"

// 当たり判定クラス いろんな判定を取れるようにしたいよね
class Judge
{
public:


	Judge(){};
	~Judge();

	static bool c_b(const Object3d& capsle, const Object3d& sphere);
	static void HitJudge(Player* p, Enemy* e);	// 当たり判定を取る、ここで処理を行う

	static Judge* getInstance()
	{
		static Judge* instance;
		return instance;
	}
};