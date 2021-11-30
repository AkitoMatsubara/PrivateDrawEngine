#include "Stages.h"

#include <algorithm>	// std::generate�p

#include "misc.h"	// �G���[�o�͗p

StageParts::StageParts()
{
}

void StageParts::Initialize()
{
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");

	Parameters = std::make_unique<Object3d>();

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
	Model->Render(SkinnedShader.get());
}

void StageParts::onObject(const DirectX::SimpleMath::Vector3& obj)
{
	// �܂��X�e�[�W�̎O�p�`���_���Z�o���܂�
	// ���f���ς������蒼���K�v���邩�炿����Ƃ܂�������
	DirectX::SimpleMath::Vector3 triangle[3];
	triangle[0] = (Model->getWorld().Backward() * 1.0f);	// �O���̓_�̂���
	triangle[1] = (Model->getWorld().Forward() * 1.0f) + (Model->getWorld().Left() * 0.5f);	// �E���̓_�̂���
	triangle[2] = (Model->getWorld().Forward() * 1.0f) + (Model->getWorld().Right() * 0.5f);	// �����̓_�̂���
	/*	�����Ȃ��Ă����		*/
	/*		     *[0]			*/
	/*		    / \				*/
	/*		   /   \			*/
	/*		  /     \			*/
	/*		 /       \			*/
	/*	  [2]*--------*[1]		*/



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
		it->Update();
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

}
