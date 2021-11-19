#include "Stage.h"

#include "misc.h"	// �G���[�p

Stage::Stage(const char* fbx_filename, int cstNo, const bool triangulate)
{
	if ((ROW_PARTS % 2 == 0) || (COL_PARTS % 2 == 0))	// ������Ȃ�
	{
		_ASSERT_EXPR_A(false, "StageParts is not an odd number.");	// �X�e�[�W�p�[�c����ł͗L��܂���G���[
	}
	for (int i = 0; i < PARTS_SIZE; i++)
	{
		Model[i] = std::make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);
	}
	//Model = make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);
}

void Stage::Initialize()
{
	SkinnedShader = std::make_unique<ShaderEx>();
	SkinnedShader->Create(L"Shaders\\skinned_mesh_vs", L"Shaders\\skinned_mesh_ps");
	//---------------------------------------------------------
	static const int CENTER = PARTS_SIZE / 2;	// �c������̂Ƃ��A/2+1�Ő^�񒆂̐��l������c�񂾂��ǁA0�X�^�[�g�̂Ƃ��͕��ʂɔ����̒l

	// �^�񒆂�0�Ƃ��邽�߁A�^�񒆂���̍������J�n�n�_�A�I�_�n�_�ɂ����Ă���B�u1,2,3�v�Ō�����2��^�񒆁A1��3���n�_�I�_�ɂ���B
	// �(��L3)�������Ɉ�x�ϊ�����̂�-1���Ă���B
	for (int i = -((COL_PARTS - 1) * 0.5f); i <= ((COL_PARTS - 1) * 0.5f); i++)
	{
		for (int j = -((ROW_PARTS - 1) * 0.5f); j <= ((ROW_PARTS - 1) * 0.5f); j++)
		{
			// �^�񒆂̗v�f(CENTER)�ɍs(j)�����Z���A��(COL_PARTS)��i�񕪏�Z���Č��Z����
			Model[CENTER + j + (COL_PARTS * i)]->setPos(j * 1.0f, 0.0f, -2.0f * i);

			static bool inversion = false;	// �p�x���]�t���O
			inversion = (j % 2) ? true : false;	// ������s�ڂȂ�p�x���]�t���O��true�ɂ���
			if (i % 2)inversion = !inversion;	// ���A���ڂȂ�p�x���]�t���O�́u�����v�𔽓]������
			if (inversion)	// 180����]������
			{
				Model[CENTER + j + (COL_PARTS * i)]->setAngle(0.0f, 180.0f, 0.0);
				Model[CENTER + j + (COL_PARTS * i)]->setColor(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				Model[CENTER + j + (COL_PARTS * i)]->setAngle(0.0f, 0.0f, 0.0f);
				Model[CENTER + j + (COL_PARTS * i)]->setColor(0.0f, 0.0f, 1.0f, 1.0f);
			}
		}
	}
	// 3*3�̂Ƃ� center=4 i=0 j=0
	Model[CENTER]->setColor(1.0f, 1.0f, 0.0f, 1.0f);
	////---------------------------------------------------------
}

void Stage::Update()
{
}

void Stage::Render()
{
	for (int i = 0; i < PARTS_SIZE; i++)
	{
		Model[i]->Render(SkinnedShader.get());
	}

	//Model->Render(SkinnedShader.get());
}

void Stage::setStageFbx(const char* fbx_filename, int cstNo, const bool triangulate)
{
	for (int i = 0; i < PARTS_SIZE; i++)
	{
		Model[i] = std::make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);
	}

	//if (Model)Model = nullptr;	// �����������
	//Model = std::make_unique<Skinned_Mesh>(fbx_filename, cstNo, triangulate);	// �Đ���
}