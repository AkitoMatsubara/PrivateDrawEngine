#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"
#include "Singleton.h"

#include <memory>

class Enemy {
	// �ϐ�
private:

	// ���f�����
	std::unique_ptr<Skinned_Mesh> Model;

	// �����蔻�����
	std::unique_ptr<Geometric_Capsule> Capcule;

	Object3d Target;	// �_������ �ʒu

	enum ENEMYSTATE
	{
		GOTARGET,	// �^�[�Q�b�g�����֐i��
		SHOT,		// �O���Ɍ���
		END			// �����Ȃ�

	};
	ENEMYSTATE state;	// �������X�e�[�g�������邽��

	float ShotInterval;	// ���ˊԊu
	float interval = 0.0f;
	bool shoted = false;

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

	/// <summary>�^�[�Q�b�g�����֌���</summary>
	/// <param name="focusAngle">����p�x 0.0f�`360.0f</param>
	/// <param name="focusRange">���E����</param>
	void FocusTarget(float focusAngle,float focusRange);

	/// <summary>�e������</summary>
	void Shot();



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
	void Render();

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