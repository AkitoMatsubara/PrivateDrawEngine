#pragma once

//#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Object3d.h"

// �����蔻��N���X �����Ȕ��������悤�ɂ��������
class Judge
{
public:


	Judge(){};
	~Judge();

	static bool c_b(const Object3d& capsle, const Object3d& sphere);
	static void HitJudge(Player* p, Enemy* e);	// �����蔻������A�����ŏ������s��

	static Judge* getInstance()
	{
		static Judge* instance;
		return instance;
	}
};