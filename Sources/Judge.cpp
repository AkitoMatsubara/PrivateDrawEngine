#include "Judge.h"
#include "XMFLOAT_Helper.h"

bool Judge::c_b(const Object3d& capsle, const int height,const Object3d& sphere)
{
	DirectX::SimpleMath::Vector3 forward;	// �J�v�Z����ʂ̈ʒu
	DirectX::SimpleMath::Vector3 back;		// �J�v�Z�����ʂ̈ʒu
	static float half = 1.5f;
	forward =	capsle.Position + (capsle.Vector * (capsle.Scale * height));	// �O���փY�����ăJ�v�Z����ʂƂ��Ďg�� �������̓T�C�Y�I�ȈӖ�����
	back =		capsle.Position - (capsle.Vector * (capsle.Scale * height));	// ����փY�����ăJ�v�Z�����ʂƂ��Ďg�� �������̓T�C�Y�I�ȈӖ�����

	DirectX::SimpleMath::Vector3 len1 = sphere.Position - forward;
	DirectX::SimpleMath::Vector3 len2 = sphere.Position - back;
	float d1 = len1.Length();	// �O������
	float d2 = len2.Length();	// �������
	static const float RADIAN = 0.5f;	// ���݌Œ� ���[��c
	if (d2 <= d1)
	{
		// �X�P�[���͑S�v�f�����l�Ȃ̂łƂ肠�����ǂ��ł�����
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
	DirectX::SimpleMath::Vector3 forward;	// �J�v�Z����ʂ̈ʒu
	DirectX::SimpleMath::Vector3 back;		// �J�v�Z�����ʂ̈ʒu
	static float half = 1.5f;
	forward = capsle.Parameters->Position + (capsle.Parameters->Vector * (capsle.Parameters->Scale * (capsle.Height*0.5f)));	// �O���փY�����ăJ�v�Z����~�_�Ƃ��Ďg�� �������̓T�C�Y�I�ȈӖ�����
	back =    capsle.Parameters->Position - (capsle.Parameters->Vector * (capsle.Parameters->Scale * (capsle.Height*0.5f)));	// ����փY�����ăJ�v�Z�����~�_�Ƃ��Ďg�� �������̓T�C�Y�I�ȈӖ�����

	DirectX::SimpleMath::Vector3 len1 = sphere.Parameters->Position - forward;
	DirectX::SimpleMath::Vector3 len2 = sphere.Parameters->Position - back;
	float d1 = len1.LengthSquared();	// �O������
	float d2 = len2.LengthSquared();	// �������

	if (d2 <= d1)
	{
		// sphere�͋��ŃX�P�[���͑S�v�f�����l�Ȃ̂łǂ��ł�����
		// �Ȃ��0.5�{�����炢�������H�킩���c
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