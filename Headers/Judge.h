#pragma once
#include <memory>

#include "Object3d.h"
#include "geometric_primitive.h"

// 当たり判定クラス いろんな判定を取れるようにしたいよね
class Judge
{
public:


	Judge(){};
	~Judge();

	static bool c_b(const Object3d& capsle, const float offset, const Object3d& sphere);

	static Judge* getInstance()
	{
		static Judge* instance;
		return instance;
	}
};