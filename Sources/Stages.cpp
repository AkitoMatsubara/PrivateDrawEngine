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

void StageParts::setStageFbx(const char* fbx_filename, int cstNo, const bool triangulate)
{
	Model = std::make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);
}

int StageParts::getStageObj()
{
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
StageManager::StageManager()
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// ������Ȃ�
	{
		_ASSERT_EXPR_A(false, "StageParts is not an odd number.");	// �X�e�[�W�p�[�c����ł͗L��܂���G���[
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
			Stages[CENTER + j + (COL_PARTS * i)]->Initialize();
			// �^�񒆂̗v�f(CENTER)�ɍs(j)�����Z���A��(COL_PARTS)��i�񕪏�Z���Č��Z����
			Stages[CENTER + j + (COL_PARTS * i)]->Parameters->Position = DirectX::SimpleMath::Vector3(j * 1.0f, -0.5f, -2.0f * i);

			bool inversion = false;	// �p�x���]�t���O
			inversion = (j % 2) ? true : false;	// �������A��s�ڂȂ�p�x���]�t���O��true�ɂ���
			if (i % 2)inversion = !inversion;	// �������A���ڂȂ�p�x���]�t���O�́u�����v�𔽓]������
			if (inversion)	// 180����]������
			{
				Stages[CENTER + j + (COL_PARTS * i)]->Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 180.0f, 0.0);
				Stages[CENTER + j + (COL_PARTS * i)]->Parameters->Color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Stages[CENTER + j + (COL_PARTS * i)]->Parameters->Rotate = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
				Stages[CENTER + j + (COL_PARTS * i)]->Parameters->Color = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			}
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