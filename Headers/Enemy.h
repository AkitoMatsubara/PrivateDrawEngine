#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"

#include <memory>



class Enemy {
	// �ϐ�
private:

	// ���f�����
	std::unique_ptr<Skinned_Mesh> Model;
	// �f�t�H���g�̃V�F�[�_�[
	std:: unique_ptr<ShaderEx> SkinnedShader = nullptr;

	std::unique_ptr<Shot> Shots;
protected:
public:
		std::unique_ptr<Object3d> Parameters;
// �֐�
private:
	void Move(); // ����֐�
protected:
public:
	 Enemy() {};
	~Enemy() {};

	void Initialize();
	void Update();
	void Render();

};