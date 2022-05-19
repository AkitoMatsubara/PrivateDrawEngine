#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"
#include "GPUParticle.h"

#include <memory>

class Enemy {
	// 変数
private:

	// モデル情報
	std::unique_ptr<Skinned_Mesh> Model;

	// 当たり判定可視化
	std::unique_ptr<Geometric_Capsule> Capcule;

	Object3d Target;	// 狙う相手 位置

	enum class ENEMYSTATE
	{
		GOTARGET,	// ターゲット方向へ進む
		SHOT,		// 前方に撃つ
		END			// 動かない

	};
	ENEMYSTATE state;	// 挙動をステート分けするため

	float ShotInterval;	// 発射間隔指定
	float interval;	// 発射してからいくらか
	bool shoted;	// 撃ったかどうか

	bool death;	// 体力が尽きたかどうか

public:
	std::unique_ptr<Object3d> Parameters;
	// 関数
private:
	void Move(); // 挙動制御関数
	void GoStraight();	// ターゲットへ直進行動

public:
	void Initialize();
	void Update();
	void Render(ID3D11DeviceContext* device_context, Shader* shader = nullptr);

	/// <summary>ターゲット方向へ向く</summary>
	/// <param name="focusAngle">視野角度 0.0f～360.0f</param>
	/// <param name="focusRange">視界距離</param>
	void FocusTarget(float focusAngle,float focusRange);

	/// <summary>弾のセットから発射、床ダメージ処理を呼び出す</summary>
	/// <summary>あと発射間隔の管理</summary>
	void Shot();

	// 被撃墜演出
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
	std::unique_ptr<ShotManager> shotsManager;	// EnemyManager内で管理したほうが良い気がした
public:

	void Initialize();
	void Update();	// 存在していない弾は内部で削除している
	void Render(ID3D11DeviceContext* device_context, Shader* shader = nullptr);

	static EnemyManager& getInstance()
	{
		static EnemyManager instance;
		return instance;
	}

	// 生成、格納系
	void newSet(const Object3d* initData);
	void push(std::unique_ptr<Enemy> enemy) { Enemys.emplace_back(std::move(enemy)); };	// 配列に格納する

	ShotManager* getShotManager() { return shotsManager.get(); }
	size_t getSize() { return Enemys.size(); };
	std::vector<std::unique_ptr<Enemy>>* getEnemys() { return &Enemys; }	// 格納コンテナを返す 外部から参照したいときに
};