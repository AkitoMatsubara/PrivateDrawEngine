#pragma once


#include "instance_skinned_mesh.h"
#include "skinned_mesh.h"
#include "Object3d.h"
#include "shaderEx.h"

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
	static inline std::unique_ptr<InstanceSkinnedMesh> InstanceModel;	// �C���X�^���X�`�悷�郂�f��

	int draw_amount;	// �C���X�^���X�`��p �`�悷��񐔂��w��
	//�֐�
private:
public:
	StageParts(int n) :draw_amount(n) {};	// �R���X�g���N�^�ŃC���X�^���X�`�搔�����߂�
	~StageParts() {};

	void Initialize();
	void Update(bool inst);
	void Render(UINT drawCount, bool inst, Shader* shader = 0);

	//bool onObject(const Object3d& obj);	// �O�p�`���f���̂Ƃ� �O���ɏ������炵�Ĕ�����Ƃ��Ă�
	bool onObjectSphere(const Object3d& obj, const float& radian);	// �����f�����̔���
	void Damage();
};

// �X�e�[�W�p�[�c�̑��Ǘ��N���X �Ƃ������p�[�c�S�������Ă�
class StageManager
{
private:
	const static int ROW_PARTS = 19;	// �s���B��ł̐ݒ肨�肢���܂�
	const static int COL_PARTS = 19;	// �񐔁B��ł̐ݒ肨�肢���܂�
	const static int PARTS_SIZE = ROW_PARTS * COL_PARTS;	// �c*���̑S�Ẵp�[�c��

	std::array<std::unique_ptr<StageParts>, PARTS_SIZE> Stages;	// ���̂܂܃��f��
	static inline bool inst = true;	// �C���X�^���X�`��̗L��

public:
	StageManager();
	void Initialize();
	void Update();
	void Render(Shader* shader = nullptr);

	static StageManager& getInstance()
	{
		static StageManager instance;
		return instance;
	}
	// StageParts�̏�Œe�𔭎˂����Ƃ��ɌĂяo���B
	///<summary>�ǂ̏��ɏ���Ă邩����A�_���[�W���Z</summary>
	void Check(const Object3d& obj, const float& radian);

	/// <summary>���̏�ɏ���Ă��邩�̔���</summary>
	bool RideParts(Object3d& obj, const float& radian);

	// �擾�A�i�[�n
	size_t getSize() { return Stages.size(); };
};