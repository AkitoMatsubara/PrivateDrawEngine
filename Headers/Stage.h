#pragma once

#include "skinned_mesh.h"
#include "shaderEx.h"

#include <memory>

class Stage
{
	// �ϐ�
private:
	// �f�t�H���g�̃V�F�[�_�[
	std::unique_ptr<ShaderEx> SkinnedShader;

public:
	std::unique_ptr<Skinned_Mesh> Model;	// ���̂܂܃��f��

	//�֐�
private:
public:
	Stage(const char* fbx_filename = ".\\Resources\\Stage\\Stage.fbx", int cstNo = 0, const bool triangulate = false);
	~Stage() {};

	void Initialize();
	void Update();
	void Render();

	void setStageFbx(const char* fbx_filename, int cstNo = 0, const bool triangulate = false);	// SkinnedMesh�̃C�j�V�����C�U�����̂܂܌Ăяo��
};