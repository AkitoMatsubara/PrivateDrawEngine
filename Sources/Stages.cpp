#include "Stages.h"

#include <algorithm>	// std::generate�p

#include "misc.h"	// �G���[�o�͗p

StageParts::StageParts(){}

StageParts::~StageParts(){}

void StageParts::Initialize()
{
	Parameters = std::make_unique<Object3d>();
	Parameters->MaxLife = 5;
	Parameters->CurLife = Parameters->MaxLife;
	Parameters->Exist = true;
	Model = std::make_unique<Skinned_Mesh>(".\\Resources\\Stage\\Stage.fbx", 0, false);
}

void StageParts::Update()
{
	// �M�~�b�N�g�ݍ���
	// �e��������object�����m�A�����������Ă���X�e�[�W�����C�t���
	// ���C�t���Ȃ��Ȃ�Ώ���������������Ċ���

	// �`�悷�邽�߃��f���ɏ���n��
	Model->getParameters()->CopyParam(Parameters.get());
}

void StageParts::Render()
{
	if (Parameters->Exist) {
		Model->Render();
	}
}


bool StageParts::onObject(const Object3d& obj)
{
	// �v�Z���׌y���̂���(�ɂȂ邩������Ȃ�����)���苗���𐧌�
	DirectX::SimpleMath::Vector3 length = Parameters->Position - obj.Position;
	if (length.Length() > 2.5f) return false;	// ���ؓI��2.5f�͎w�肪�o�O�Ȃ��ėǂ�����

	DirectX::SimpleMath::Vector3 objForward = obj.Position + (obj.Vector * 0.7f);	// �v���C���[����O���ɏ������炵���ʒu �����o���������画�����邽��

	// �܂��X�e�[�W�̎O�p�`���_���Z�o���܂�
	// ��(12/09)���݂̃��f���ɍ��킹�Ă�̂ŁA�ς������蒼���K�v���邩�炿����Ƃ܂�������
	float ofs_x = 1.0f;	// ���_���獶�E�ւ̃Y��
	float ofs_z = 1.0f;	// ���_�����ւ̃Y��
	DirectX::SimpleMath::Vector3 triangle[3];
	triangle[0] = Parameters->Position + (Model->getWorld().Backward() * ofs_x);	// �O���̓_�̂���
	triangle[1] = Parameters->Position + (Model->getWorld().Forward() * ofs_x) + (Model->getWorld().Left() * ofs_z);	// �E���̓_�̂���
	triangle[2] = Parameters->Position + (Model->getWorld().Forward() * ofs_x) + (Model->getWorld().Right() * ofs_z);	// �����̓_�̂���
	/*	  �����Ȃ��Ă����	*/
	/*		   [0(A)]		�O	*/
	/*		     *			��	*/
	/*		    / \			��	*/
	/*		   /   \		��	*/
	/*		  /  P  \		��	*/
	/*		 /       \			*/
	/*[2(C)]*---------*[1(B)]	*/

	// y������xz���ʂŊO�ς��v�Z �������������g���̂Ń����_���Œ�`
	auto cross = [](const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2) {return (v1.x * v2.z) - (v1.z * v2.x); };
	DirectX::SimpleMath::Vector3 A_p, B_p, C_p, A_B, B_C, C_A;	// ���ꂼ��̃x�N�g�����i�[����ϐ�
	{
		A_p = objForward - triangle[0];		// A����obj�O���Ɍ������x�N�g��
		B_p = objForward - triangle[1];		// B����obj�O���Ɍ������x�N�g��
		C_p = objForward - triangle[2];		// C����obj�O���Ɍ������x�N�g��
		A_B = triangle[1] - triangle[0];	// A����B�Ɍ������x�N�g��
		B_C = triangle[2] - triangle[1];	// B����C�Ɍ������x�N�g��
		C_A = triangle[0] - triangle[2];	// C����A�Ɍ������x�N�g��
		A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y���͎g�p���Ȃ��̂�0�ɂ��ď���
		float c1 = cross(A_B, B_p);
		float c2 = cross(B_C, C_p);
		float c3 = cross(C_A, A_p);
		// �R�̊O�ό��ʂ�������v���Ă���Γ����A�s��v�Ȃ�O��
		if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
			return true;	// �����Ł[��
		}
	}
	// ����Ă��{�̂���ɏ���Ă邩�Čv�Z����
	{
		A_p = obj.Position - triangle[0];	// A����obj�Ɍ������x�N�g��
		B_p = obj.Position - triangle[1];	// B����obj�Ɍ������x�N�g��
		C_p = obj.Position - triangle[2];	// C����obj�Ɍ������x�N�g��
		A_p.y = B_p.y = C_p.y = A_B.y = B_C.y = C_A.y = 0.0f;	// Y���͎g�p���Ȃ��̂�0�ɂ��ď���
		float c1 = cross(A_B, B_p);
		float c2 = cross(B_C, C_p);
		float c3 = cross(C_A, A_p);
		if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
			return true;	// �����Ł[��
		}
	}
	return false;
}
void StageParts::Damage()
{
	Parameters->CurLife--;	// �̗͂����炷
	Parameters->Color.w = static_cast<float>(Parameters->CurLife) / static_cast<float>(Parameters->MaxLife);

	// �����̗͂�0�ȉ��ł���Α��݃t���O���ւ��܂�
	if (Parameters->CurLife <= 0)
	{
		Parameters->Exist = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
StageManager::StageManager()
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// ������Őݒ肵������Ă���Ȃ�
	{
		_ASSERT_EXPR_A(false, "_StageParts is not an odd number._");	// �X�e�[�W�p�[�c����ł͗L��܂���G���[
	}
}

void StageManager::Initialize()
{
	// �X�e�[�W����(����)
	//---------------------------------------------------------
	static const int CENTER = PARTS_SIZE / 2;	// �c������̂Ƃ��A/2+1�Ő^�񒆂̐��l������c�񂾂��ǁA0�X�^�[�g�̂Ƃ��͕��ʂɔ����̒l

	// ���܂�����make_unique���g�����ɒT���Ă��Ƃ���A���̂悤�ȕ��@���������̂ō̗p�B1�s�ōςނ��X�}�[�g����
	// �쐬 begin~end�܂�{}�����s����
	std::generate(std::begin(Stages), std::end(Stages), []() { return std::make_unique<StageParts>(); });

	// �^�񒆂�0�Ƃ��邽�߁A�^�񒆂���̍������J�n�n�_�A�I�_�n�_�ɂ����Ă���B�u1,2,3�v�Ō�����2��^�񒆁A1��3���n�_�I�_�ɂ���B
	// �(��L3)�������Ɉ�x�ϊ�����̂�-1���Ă���B
	for (int i = static_cast<int>(-((COL_PARTS - 1) * 0.5f)); i <= static_cast<int>(((COL_PARTS - 1) * 0.5f)); i++)
	{
		for (int j = static_cast<int>(-((ROW_PARTS - 1) * 0.5f)); j <= static_cast<int>(((ROW_PARTS - 1) * 0.5f)); j++)
		{
			int row = CENTER + j;		// �\�ߗv�f�ԍ��̌v�Z���s���Ă���
			int col = COL_PARTS * i;	// �L���X�g���Čx�����������߂��̂܂ܓ��ꂿ�Ⴄ�Ɖǐ����������Ă��܂�����
			if (row + col < 0) { _ASSERT_EXPR_A(false, "Initialize ArraySize UnderFlow"); }	// �v�f�ԍ����}�C�i�X�w��ɂȂ����������G���[

			Stages[row + col]->Initialize();
			// �^�񒆂̗v�f(CENTER)�ɍs(j)�����Z���A��(COL_PARTS)��i�񕪏�Z���Č��Z����
			Stages[row + col]->Parameters->Position = DirectX::SimpleMath::Vector3(j * 1.0f, -0.5f, -2.0f * i);

			bool inversion = false;	// �p�x���]�t���O
			inversion = (j % 2) ? true : false;	// �������A��s�ڂȂ�p�x���]�t���O��true�ɂ���
			if (i % 2)inversion = !inversion;	// �������A���ڂȂ�p�x���]�t���O�́u�����v�𔽓]������
			if (inversion)	// 180����]������
			{
				Stages[row + col]->Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 180.0f, 0.0);
				Stages[row + col]->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Stages[row + col]->Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
				Stages[row + col]->Parameters->Color = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			}
			// �ꉞ�O���Z�o
			Stages[row + col]->Parameters->calcForward();
		}
	}
	// �^�񒆂��킩��₷���F�ς��Ă�
	Stages[CENTER]->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	////---------------------------------------------------------
}

void StageManager::Update()
{
	for (auto& it : Stages)
	{
		if(it->Parameters->Exist)
		{
		it->Update();
		}
	}
}

void StageManager::Render()
{
	for (auto& it : Stages)
	{
		it->Render();
	}
}

void StageManager::Check(const Object3d& obj)
{
	for (auto& it : Stages)
	{
		// �I�u�W�F�N�g�ƃX�e�[�W�̈ʒu����ŏ�ɏ���Ă�����X�e�[�W�̃_���[�W�������Ăяo��
		// �R�����g�A�E�g�̓f�o�b�O�p�� ���₷�����Ă�
		//if(Stages[PARTS_SIZE / 2]->onObject(obj))
		if(it->onObject(obj))
		{
			it->Damage();
			//it->Parameters->Color.y = 1.0f;

		}
		//else{ it->Parameters->Color.y = 0.0f; }
	}
}
