#include "Stages.h"

#include <algorithm>	// std::generate�p

#include "misc.h"	// �G���[�o�͗p

StageParts::StageParts(){}

void StageParts::Initialize()
{
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

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
		Model->Render(SkinnedShader.get());
	}
}

void StageParts::onObject(const DirectX::SimpleMath::Vector3& objPos)
{
	const static float CHECK_DIST = 1.25f;
	DirectX::SimpleMath::Vector3 length;
	length = Parameters->Position - objPos;
	length.y = 0.0f;	// y���͖�������
	if (length.Length() > CHECK_DIST)return;

	// �܂��X�e�[�W�̎O�p�`���_���Z�o���܂�
	// ���f���ς������蒼���K�v���邩�炿����Ƃ܂�������
	float ofs_x = 1.0f;
	float ofs_y = 1.0f;
	DirectX::SimpleMath::Vector3 triangle[3];
	triangle[0] = (Model->getWorld().Backward() * ofs_x);	// �O���̓_�̂���
	triangle[1] = (Model->getWorld().Forward() * ofs_x) + (Model->getWorld().Left()  * ofs_y);	// �E���̓_�̂���
	triangle[2] = (Model->getWorld().Forward() * ofs_x) + (Model->getWorld().Right() * ofs_y);	// �����̓_�̂���
	/*	�����Ȃ��Ă����		*/
	/*		    [0]			�O	*/
	/*		     *			��	*/
	/*		    / \			��	*/
	/*		   /   \		��	*/
	/*		  /  P  \		��	*/
	/*		 /       \			*/
	/*	  [2]*--------*[1]		*/

	DirectX::SimpleMath::Vector3 p_0 = triangle[0] - objPos;
	DirectX::SimpleMath::Vector3 p_1 = triangle[1] - objPos;
	DirectX::SimpleMath::Vector3 p_2 = triangle[2] - objPos;
	p_0.y = p_1.y = p_2.y = 0.0f;	// Y���͎g�p���Ȃ��̂�0��

	float theta[3] = { 0 };
	// �Ȃ��p�Ƃ̌v�Z cos�� = (dot(a,b)/a.length * b.length)
	theta[0] = acos(p_0.Dot(p_1) / (p_0.Length() * p_1.Length()));
	theta[1] = acos(p_1.Dot(p_2) / (p_1.Length() * p_2.Length()));
	theta[2] = acos(p_0.Dot(p_2) / (p_0.Length() * p_2.Length()));
	// �m�F�p��3�_�̂Ȃ��p�����v
	float sum = 0;
	for (auto& t : theta) { sum += t; }
	// �����
	// �Ȃ��p�̍��v�l��361�������ł���Δ���
	// �Ȃ������Ȃ̂��͌덷��360.001���݂����ɂȂ����Ⴄ���炻�̑΍�
	if (sum < DirectX::XMConvertToRadians(361))
	{		 // �����_�ł���ΑS��180���ȉ��Ɏ��܂�̂Ń`�F�b�N
		if (theta[0] <= DirectX::XMConvertToRadians(180) && theta[1] <= DirectX::XMConvertToRadians(180) && theta[2] <= DirectX::XMConvertToRadians(180))
		{
			Parameters->CurLife--;	// �̗͌��炷
			Parameters->Color.w =static_cast<float>(Parameters->CurLife)/ static_cast<float>(Parameters->MaxLife);
		}
	}
	if (Parameters->CurLife <= 0)Parameters->Exist = false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
StageManager::StageManager()
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// ������Ȃ�
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

void StageManager::Check(const DirectX::SimpleMath::Vector3& obj)
{
	for (auto& it : Stages)
	{
		it->onObject(obj);
	}
	//Stages[PARTS_SIZE / 2]->onObject(obj);
}
