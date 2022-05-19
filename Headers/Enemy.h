#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"
#include "GPUParticle.h"

#include <memory>

class Enemy {
	// �ϐ�
private:

	// ���f�����
	std::unique_ptr<Skinned_Mesh> Model;

	// �����蔻�����
	std::unique_ptr<Geometric_Capsule> Capcule;

	Object3d Target;	// �_������ �ʒu

	enum class ENEMYSTATE
	{
		GOTARGET,	// �^�[�Q�b�g�����֐i��
		SHOT,		// �O���Ɍ���
		END			// �����Ȃ�

	};
	ENEMYSTATE state;	// �������X�e�[�g�������邽��

	float ShotInterval;	// ���ˊԊu�w��
	float interval;	// ���˂��Ă��炢���炩
	bool shoted;	// ���������ǂ���

	bool death;	// �̗͂��s�������ǂ���

public:
	std::unique_ptr<Object3d> Parameters;
	// �֐�
private:
	void Move(); // ��������֐�
	void GoStraight();	// �^�[�Q�b�g�֒��i�s��

public:
	void Initialize();
	void Update();
	void Render(ID3D11DeviceContext* device_context, Shader* shader = nullptr);

	/// <summary>�^�[�Q�b�g�����֌���</summary>
	/// <param name="focusAngle">����p�x 0.0f�`360.0f</param>
	/// <param name="focusRange">���E����</param>
	void FocusTarget(float focusAngle,float focusRange);

	/// <summary>�e�̃Z�b�g���甭�ˁA���_���[�W�������Ăяo��</summary>
	/// <summary>���Ɣ��ˊԊu�̊Ǘ�</summary>
	void Shot();

	// �팂�ĉ��o
	bool Destroy();

	void setDeath(bool flg) { death = flg; }

	bool getExist() { return Parameters->Exist; }
	void setExist(const bool flg) { Parameters->Exist = flg; }

	void setTarget(const Object3d &target) { Target = target; }
	const Object3d* getTarget() { return &Target; }
};

class EnemyManager
{
private:
	std::vector<std::unique_ptr<Enemy>> Enemys;
	std::unique_ptr<ShotManager> shotsManager;	// EnemyManager���ŊǗ������ق����ǂ��C������
public:

	void Initialize();
	void Update();	// ���݂��Ă��Ȃ��e�͓����ō폜���Ă���
	void Render(ID3D11DeviceContext* device_context, Shader* shader = nullptr);

	static EnemyManager& getInstance()
	{
		static EnemyManager instance;
		return instance;
	}

	// �����A�i�[�n
	void newSet(const Object3d* initData);
	void push(std::unique_ptr<Enemy> enemy) { Enemys.emplace_back(std::move(enemy)); };	// �z��Ɋi�[����

	ShotManager* getShotManager() { return shotsManager.get(); }
	size_t getSize() { return Enemys.size(); };
	std::vector<std::unique_ptr<Enemy>>* getEnemys() { return &Enemys; }	// �i�[�R���e�i��Ԃ� �O������Q�Ƃ������Ƃ���
};