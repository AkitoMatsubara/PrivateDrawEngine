#pragma once
#include <memory>

#include "Object3d.h"
#include "geometric_primitive.h"

// “–‚½‚è”»’èƒNƒ‰ƒX ‚¢‚ë‚ñ‚È”»’è‚ðŽæ‚ê‚é‚æ‚¤‚É‚µ‚½‚¢‚æ‚Ë
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