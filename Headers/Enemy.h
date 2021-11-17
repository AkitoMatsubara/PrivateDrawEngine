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
	//std::shared_ptr<Skinned_Mesh> Model;
	// �f�t�H���g�̃V�F�[�_�[
	std::unique_ptr<ShaderEx> SkinnedShader = nullptr;

	// �����蔻�����
	std::unique_ptr<Geometric_Capsule> Capcule;

	std::unique_ptr<Shot> Shots;

	bool Exist;
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

	static Enemy* getInstance()
	{
		static Enemy* instance;
		return instance;
	}

	bool getExist() { return Exist; }
	void setExist(const bool flg) { Exist = flg; }
};

class EnemyManager
{
private:
	std::vector<std::unique_ptr<Enemy>> Enemys;

public:

	void Initialize() { Enemys.clear(); }
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

	size_t getSize() { return Enemys.size(); };
	std::vector<std::unique_ptr<Enemy>>* getEnemys() { return &Enemys; }	// �i�[�R���e�i��Ԃ� �O������Q�Ƃ������Ƃ���
};