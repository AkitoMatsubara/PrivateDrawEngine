#pragma once

#include "geometric_primitive.h"
#include "shaderEx.h"

#include <memory>

class Stage
{
	// �ϐ�
private:
	std::unique_ptr<Geometric_Cylinder> Model;	// ���̂܂܃��f��
	// �f�t�H���g�̃V�F�[�_�[
	std::unique_ptr<ShaderEx> GeomtricShader;

public:
	//�֐�
private:
	Stage();
	~Stage() {};

	void  Initialize();
	void Update();
	void Render();


};