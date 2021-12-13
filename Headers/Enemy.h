#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"
#include "Singleton.h"

#include <memory>

class Enemy {
	// •Ï”
private:

	// ƒ‚ƒfƒ‹î•ñ
	std::unique_ptr<Skinned_Mesh> Model;

	// “–‚½‚è”»’è‰Â‹‰»
	std::unique_ptr<Geometric_Capsule> Capcule;

	std::unique_ptr<Shot> Shots;
	std::unique_ptr<ShotManager> ShotsManager;

	Object3d Target;	// ‘_‚¤‘Šè
protected:
public:
	std::unique_ptr<Object3d> Parameters;
	// ŠÖ”
private:
	void Move(); // §ŒäŠÖ”
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

	bool getExist() { return Parameters->Exist; }
	void setExist(const bool flg) { Parameters->Exist = flg; }

	void setTarget(const Object3d &target) { Target = target; }
};

class EnemyManager
{
private:
	std::vector<std::unique_ptr<Enemy>> Enemys;

public:

	void Initialize() { Enemys.clear(); }
	void Update();	// ‘¶İ‚µ‚Ä‚¢‚È‚¢’e‚Í“à•”‚Åíœ‚µ‚Ä‚¢‚é
	void Render();

	static EnemyManager& getInstance()
	{
		static EnemyManager instance;
		return instance;
	}

	// ¶¬AŠi”[Œn
	void newSet(const Object3d* initData);
	void push(std::unique_ptr<Enemy> enemy) { Enemys.emplace_back(std::move(enemy)); };	// ”z—ñ‚ÉŠi”[‚·‚é

	size_t getSize() { return Enemys.size(); };
	std::vector<std::unique_ptr<Enemy>>* getEnemys() { return &Enemys; }	// Ši”[ƒRƒ“ƒeƒi‚ğ•Ô‚· ŠO•”‚©‚çQÆ‚µ‚½‚¢‚Æ‚«‚É
};