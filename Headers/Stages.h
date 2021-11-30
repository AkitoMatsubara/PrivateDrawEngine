#pragma once


#include "skinned_mesh.h"
#include "Object3d.h"
#include "shaderEx.h"

#include <memory>
#include <array>
#include <vector>

class Enemy;

class StageParts
{
	// �ϐ�
private:
	// �f�t�H���g�̃V�F�[�_�[
	static inline std::unique_ptr<ShaderEx> SkinnedShader;

public:
	std::unique_ptr<Object3d> Parameters;
	std::unique_ptr<Skinned_Mesh> Model;	// ���̂܂܃��f��

	//�֐�
private:
public:
	StageParts();
	~StageParts() {};

	void Initialize();
	void Update();
	void Render();

	void setStageFbx(const char* fbx_filename, int cstNo = 0, const bool triangulate = false);	// SkinnedMesh�̃C�j�V�����C�U�����̂܂܌Ăяo��

	int getStageObj();
};
// �X�e�[�W�p�[�c�̑��Ǘ��N���X �Ƃ������p�[�c�S�������Ă�
class StageManager
{
private:
	const static int ROW_PARTS = 11;	// �s���B��ł̐ݒ肨�肢���܂�
	const static int COL_PARTS = 11;	// �񐔁B��ł̐ݒ肨�肢���܂�
	const static int PARTS_SIZE = ROW_PARTS * COL_PARTS;	// �c*���̑S�Ẵp�[�c��

	std::array<std::unique_ptr<StageParts>, PARTS_SIZE> Stages;	// ���̂܂܃��f��

public:
	StageManager();
	void Initialize();
	void Update();
	void Render();

	static StageManager& getInstance()
	{
		static StageManager instance;
		return instance;
	}

	// �����A�i�[�n
	void newSet(const Object3d* initData);

	size_t getSize() { return Stages.size(); };
};