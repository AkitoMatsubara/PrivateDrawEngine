#pragma once

#include "Object3d.h"
#include "skinned_mesh.h"
#include "geometric_primitive.h"
#include <memory>


// �e�N���X
class Shot
{
	// �ϐ�
private:
	//static std::unique_ptr<Skinned_Mesh> Model;	// static�ɂ������������Ǐ�肭�s���Ȃ��̂ň�U�폜
	std::unique_ptr<Skinned_Mesh> Model;	// ���̂܂܃��f��
	std::unique_ptr<Geometric_Sphere> test;	// �e�X�g���f�� �܂�

	// �f�t�H���g�̃V�F�[�_�[
	std::unique_ptr<ShaderEx> SkinnedShader = nullptr;

	std::unique_ptr<Object3d> Parameters;
	float Timer;	// �ˏo���Ԃۗ̕L
	bool Exist;		// ���ˍς݃t���O
public:

	// �֐�
private:
public:
	std::unique_ptr<Shot> clone()const;

	Shot(){};
	Shot(Shot&shot)
	{
		this->Model = std::move(shot.Model);
		this->test = std::move(shot.test);
		this->Timer = shot.Timer;
	};
	~Shot(){};

	void Initialize();
	void Update();
	void Render();

	void set(const Object3d* parent);	// �I�u�W�F�N�g�͑S��Object3d�N���X��ۗL���Ă���O��Ŋe��p�����[�^���R�s�[
	bool getExist() { return Exist; }
};

