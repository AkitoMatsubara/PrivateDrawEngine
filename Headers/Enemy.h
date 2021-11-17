#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"
#include "Singleton.h"

#include <memory>

class Enemy {
	// 変数
private:

	// モデル情報
	std::unique_ptr<Skinned_Mesh> Model;
	//std::shared_ptr<Skinned_Mesh> Model;
	// デフォルトのシェーダー
	std::unique_ptr<ShaderEx> SkinnedShader = nullptr;

	// 当たり判定可視化
	std::unique_ptr<Geometric_Capsule> Capcule;

	std::unique_ptr<Shot> Shots;

	bool Exist;
protected:
public:
	std::unique_ptr<Object3d> Parameters;
	// 関数
private:
	void Move(); // 制御関数
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
	void Update();	// 存在していない弾は内部で削除している
	void Render();

	static EnemyManager& getInstance()
	{
		static EnemyManager instance;
		return instance;
	}

	// 生成、格納系
	void newSet(const Object3d* initData);
	void push(std::unique_ptr<Enemy> enemy) { Enemys.emplace_back(std::move(enemy)); };	// 配列に格納する

	size_t getSize() { return Enemys.size(); };
	std::vector<std::unique_ptr<Enemy>>* getEnemys() { return &Enemys; }	// 格納コンテナを返す 外部から参照したいときに
};