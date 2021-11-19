#include "Judge.h"
#include "XMFLOAT_Helper.h"

bool Judge::c_b(const Object3d& capsle, const Object3d& sphere)
{
	DirectX::SimpleMath::Vector3 forward;	// �J�v�Z����ʂ̈ʒu
	forward = capsle.Position + (capsle.Vector * (capsle.Scale * 0.5f));	// �O���փY�����ăJ�v�Z����ʂƂ��Ďg�� �������̓T�C�Y�I�ȈӖ�����
	DirectX::SimpleMath::Vector3 back;		// �J�v�Z�����ʂ̈ʒu
	back = capsle.Position - (capsle.Vector * (capsle.Scale * 0.5f));		// ����փY�����ăJ�v�Z�����ʂƂ��Ďg�� �������̓T�C�Y�I�ȈӖ�����

	float d1 = Float3Length(forward - sphere.Position);	// �O������
	float d2 = Float3Length(back - sphere.Position);	// �������

	if (d2 <= d1)
	{
		// sphere�͋��ŃX�P�[���͑S�v�f�����l�Ȃ̂łǂ��ł�����
		// �Ȃ��0.5�{�����炢�������H�킩���c
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