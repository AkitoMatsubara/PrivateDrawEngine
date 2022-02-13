#pragma once


#include "skinned_mesh.h"
#include "Object3d.h"

#include <memory>
#include <array>
#include <SimpleMath.h>

class StageParts
{
	// �ϐ�
private:

public:
	std::unique_ptr<Object3d> Parameters;
	std::unique_ptr<Skinned_Mesh> Model;	// ���̂܂܃��f��

	//�֐�
private:
public:
	StageParts();
	~StageParts();

	void Initialize();
	void Update();
	void Render();

	bool onObject(const Object3d& obj);	// �O�p�`���f���̂Ƃ� �O���ɏ������炵�Ĕ�����Ƃ��Ă�
	bool onObjectSphere(const Object3d& obj,const float& radian);	// �����f�����̔���
	void Damage();
};
// �X�e�[�W�p�[�c�̑��Ǘ��N���X �Ƃ������p�[�c�S�������Ă�
class StageManager
{
private:
	const static int ROW_PARTS = 25;	// �s���B��ł̐ݒ肨�肢���܂�
	const static int COL_PARTS = 25;	// �񐔁B��ł̐ݒ肨�肢���܂�
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
	// StageParts�̏�Œe�𔭎˂����Ƃ��ɌĂяo���B
	///<summary>�ǂ̏��ɏ���Ă邩����A�_���[�W���Z</summary>
	void Check(const Object3d& obj, const float& radian);

	/// <summary>���̏�ɂ��Ȃ���Ή��ɗ����鏈��</summary>
	bool RideParts(Object3d& obj,const float& radian);

	// �擾�A�i�[�n
	size_t getSize() { return Stages.size(); };
};