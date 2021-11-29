#pragma once


#include "skinned_mesh.h"
#include "Object3d.h"
#include "shaderEx.h"

#include <memory>
#include <array>

class Stage
{
	// �ϐ�
private:
	// �f�t�H���g�̃V�F�[�_�[
	std::unique_ptr<ShaderEx> SkinnedShader;

	const static int ROW_PARTS = 5;	// �s���B��ł̐ݒ肨�肢���܂�
	const static int COL_PARTS = 5;	// �񐔁B��ł̐ݒ肨�肢���܂�
	const static int PARTS_SIZE = ROW_PARTS * COL_PARTS;	// �c*���̑S�Ẵp�[�c��
	std::array<std::unique_ptr<Skinned_Mesh>, PARTS_SIZE> Model;	// ���̂܂܃��f��
public:
	std::unique_ptr<Object3d> Parameters;

	//�֐�
private:
public:
	Stage(const char* fbx_filename = ".\\Resources\\Stage\\Stage.fbx", int cstNo = 0, const bool triangulate = false);
	~Stage() {};

	void Initialize();
	void Update();
	void Render();

	void setStageFbx(const char* fbx_filename, int cstNo = 0, const bool triangulate = false);	// SkinnedMesh�̃C�j�V�����C�U�����̂܂܌Ăяo��

	int getStageObj();
};