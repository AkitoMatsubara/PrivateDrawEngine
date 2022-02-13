#pragma once
#include "Shot.h"
#include "skinned_mesh.h"
#include "shaderEx.h"
#include "Object3d.h"

#include "geometric_primitive.h"

#include <memory>
#include <vector>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

class Player {
	// ------------------変数------------------
private:
	// モデル情報
	std::unique_ptr<Skinned_Mesh> Model;

	// カプセル型で当たり判定取るために可視化する
	std::unique_ptr<Geometric_Capsule> Capcule;
	std::unique_ptr<Geometric_Sphere> playerHitSphere;
	std::unique_ptr<Geometric_Sphere> testSphere2;

	std::unique_ptr<ShotManager> ShotsManager;
	inline static Player* instance;

public:
	std::unique_ptr<Object3d> Parameters;

	// ------------------関数------------------
private:
	void Control(); // 制御関数
public:
	Player() {};
	~Player() {};

	void Initialize();
	void Update();
	void Render();

	ShotManager* getShotManager() { return ShotsManager.get(); }
	static Player* getInstance() { return instance; }

	void ImguiPlayer();

	int Damage();	// ダメージ加算、ダメ計算後のHPを返す。死亡処理に使えると思って
};