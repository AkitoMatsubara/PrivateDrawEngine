#pragma once

//#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Object3d.h"
#include "geometric_primitive.h"
// �����蔻��N���X �����Ȕ��������悤�ɂ��������
class Judge
{
public:


	Judge(){};
	~Judge();

	static bool c_b(const Object3d& capsle,const float height, const Object3d& sphere);	// Object3d�̒l�Ŋ撣�낤�Ƃ��Ă���
	static bool JudgeCap_Sphe(const Geometric_Capsule& capsle, const Geometric_Sphere& sphere);	// Geomteric_[X]���g���Ĕ��肵�悤�Ƃ������
	static void HitJudge(Player* p, Enemy* e);	// �����蔻������A�����ŏ������s��

	static Judge* getInstance()
	{
		static Judge* instance;
		return instance;
	}
};